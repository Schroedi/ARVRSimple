////////////////////////////////////////////////////////////////////////////
// This is just an example POWERWALL GDNative module. It does very little VR-ish
// It also outputs lots of debugging stuff which should not be in a proper
// module :)

// Written by Bastiaan "Mux213" Olij, with loads of help from Thomas "Karroffel" Herzog

#include "PowerwallInterface.h"
#include "PowerwallScreen.h"
#include <cassert>
#include <iostream>
#include <cmath>

arvr_data_struct *arvr_data = nullptr;

void updateOpentrack(arvr_data_struct *arvr_data);

namespace {
const char *kName = "Powerwall";
} // namespace

void printVector(godot_vector3 *v) {
    printf("[%f, %f, %F]\n",
           api->godot_vector3_get_axis(v, godot_vector3_axis::GODOT_VECTOR3_AXIS_X),
           api->godot_vector3_get_axis(v, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y),
           api->godot_vector3_get_axis(v, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z));
}

godot_string godot_arvr_get_name(const void *p_data) {
    godot_string ret;

    printf("Powerwall interface get name.\n");

    api->godot_string_new(&ret);
    api->godot_string_parse_utf8(&ret, kName);

    return ret;
}

godot_int godot_arvr_get_capabilities(const void *p_data) {
    printf("Powerwall interface get capabilities.");
    return 2; // 2 = ARVR_STEREO
}

godot_bool godot_arvr_get_anchor_detection_is_enabled(const void *p_data) {
    // does not apply here
    return false;
}

void godot_arvr_set_anchor_detection_is_enabled(void *p_data, bool p_enable) {
    // we ignore this, not supported in this interface!
}

void godot_arvr_notification(void *p_data, godot_int p_what) {
    // we don't care
}

godot_bool godot_arvr_is_stereo(const void *p_data) {
    return true;
}

godot_bool godot_arvr_is_initialized(const void *p_data) {
    godot_bool ret;
    auto *arvr_data = (arvr_data_struct *)p_data;

    ret = arvr_data == nullptr ? false : arvr_data->is_initialised;

    return ret;
}

godot_bool godot_arvr_initialize(void *p_data) {
    godot_bool ret;
    auto * arvr_data = (arvr_data_struct *) p_data;

    printf("Powerwall.godot_arvr_initialize()\n");

    if (!arvr_data->is_initialised) {
        // initialise this interface, so initialize any 3rd party libraries, open up HMD window if required, etc.
        api->godot_vector3_set_axis(&arvr_data->pe, godot_vector3_axis::GODOT_VECTOR3_AXIS_X, 0);
        api->godot_vector3_set_axis(&arvr_data->pe, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y, 1.75);
        api->godot_vector3_set_axis(&arvr_data->pe, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z, 2);

        api->godot_quat_set_x(&arvr_data->re, 1);
        api->godot_quat_set_y(&arvr_data->re, 0);
        api->godot_quat_set_z(&arvr_data->re, 0);
        api->godot_quat_set_w(&arvr_data->re, 0);

        api->godot_string_new(&arvr_data->tracker_url);
        arvr_data->vrpnTracker = nullptr;

        arvr_data->opentrack = new OpentrackServer(4242);

        arvr_data->swap_eyes = false;

        // note, this will be made the primary interface by ARVRInterfaceGDNative
        arvr_data->is_initialised = true;
    }

    // and return our result
    ret = arvr_data->is_initialised;
    return ret;
}

void godot_arvr_uninitialize(void *p_data) {
    auto * arvr_data = (arvr_data_struct *) p_data;

    if (arvr_data->is_initialised) {
        // note, this will already be removed as the primary interface by ARVRInterfaceGDNative
        // cleanup if needed
        arvr_data->is_initialised = false;
    }
}

godot_vector2 godot_arvr_get_render_targetsize(const void *p_data) {
    /* returns the recommended render target size per eye for this device */
    //printf("Powerwall.arvr_get_recommended_render_targetsize()\n");

    godot_vector2 size;
    //api->godot_vector2_new(&size, 2560, 1600);
    api->godot_vector2_new(&size, 1920, 1080);

    return size;
}

godot_transform godot_arvr_get_transform_for_eye(void *p_data, godot_int p_eye, godot_transform *p_cam_transform) {
    // TODO: we should only return the offset to the cyclone's eye -- but nobody does that
    // the result will be used for culling

    godot_transform reference_frame = arvr_api->godot_arvr_get_reference_frame();
    godot_transform ret;

    auto *arvr_data = (arvr_data_struct *)p_data;
    assert(arvr_data != nullptr); // Invalid arvr data

    arvr_data->cam_transform = *p_cam_transform;

    godot_transform transform_for_eye;
    godot_transform hmd_transform;
    godot_vector3 offset;
    // Currently we only support 1to1 scaling.
    godot_real world_scale = 1;//arvr_api->godot_arvr_get_worldscale();

    godot_basis head_rotation;
    api->godot_basis_new_with_euler_quat(&head_rotation, &arvr_data->re);

    // create our transform from head center to eye
    api->godot_transform_new_identity(&transform_for_eye);
    if (p_eye == 1) {
        // left eye
        api->godot_vector3_new(&offset, -arvr_data->iod_m * 0.5 * world_scale, 0.0, 0.0);
        offset = api->godot_basis_xform(&head_rotation, &offset);
        transform_for_eye = api->godot_transform_translated(&transform_for_eye, &offset);
    } else if (p_eye == 2) {
        // right eye
        api->godot_vector3_new(&offset, arvr_data->iod_m * 0.5 * world_scale, 0.0, 0.0);
        offset = api->godot_basis_xform(&head_rotation, &offset);
        transform_for_eye = api->godot_transform_translated(&transform_for_eye, &offset);
    } else {
        // leave in the middle, mono
    }

    // now determine our HMD positional tracking
    api->godot_transform_new_identity(&hmd_transform);
    hmd_transform = api->godot_transform_translated(&hmd_transform, &arvr_data->pe);

    // Now construct our full transform, the order may be in reverse, have to test :)
    ret = *p_cam_transform;
    ret = api->godot_transform_operator_multiply(&ret, &reference_frame);
    ret = api->godot_transform_operator_multiply(&ret, &hmd_transform);
    ret = api->godot_transform_operator_multiply(&ret, &transform_for_eye);

    return ret; // transform; //ret;
}

godot_vector3 get_eye_pos(void *p_data, int p_eye) {
    auto *arvr_data = (arvr_data_struct *)p_data;
    assert(arvr_data != nullptr); // Invalid arvr data

    godot_transform transform_for_eye;
    godot_transform hmd_transform;
    godot_vector3 offset;
    // Currently we only support 1to1 scaling.
    godot_real world_scale = 1;//arvr_api->godot_arvr_get_worldscale();

    godot_basis head_rotation;
    //api->godot_basis_new_with_euler(&head_rotation, &arvr_data->re);
    api->godot_basis_new_with_euler_quat(&head_rotation, &arvr_data->re);

    // create our transform from head center to eye
    api->godot_transform_new_identity(&transform_for_eye);
    if (p_eye == 1) {
        // left eye
        api->godot_vector3_new(&offset, -arvr_data->iod_m * 0.5 * world_scale, 0.0, 0.0);
        offset = api->godot_basis_xform(&head_rotation, &offset);
        transform_for_eye = api->godot_transform_translated(&transform_for_eye, &offset);
    } else if (p_eye == 2) {
        // right eye
        api->godot_vector3_new(&offset, arvr_data->iod_m * 0.5 * world_scale, 0.0, 0.0);
        offset = api->godot_basis_xform(&head_rotation, &offset);
        transform_for_eye = api->godot_transform_translated(&transform_for_eye, &offset);
    } else {
        // leave in the middle, mono
    }

    // now determine our HMD positional tracking
    api->godot_transform_new_identity(&hmd_transform);
    hmd_transform = api->godot_transform_translated(&hmd_transform, &arvr_data->pe);

    // Now construct our full transform, the order may be in reverse, have to test :)
    godot_transform reference_frame = arvr_api->godot_arvr_get_reference_frame();
    godot_transform transform = arvr_data->cam_transform;
    transform = api->godot_transform_operator_multiply(&transform, &reference_frame);
    transform = api->godot_transform_operator_multiply(&transform, &hmd_transform);
    transform = api->godot_transform_operator_multiply(&transform, &transform_for_eye);
    godot_vector3 ret;
    api->godot_vector3_new(&ret,0,0,0);
    ret = api->godot_transform_xform_vector3(&transform, &ret);
    return ret;
}

void arvr_set_frustum(godot_real *p_projection, godot_real p_left, godot_real p_right, godot_real p_bottom, godot_real p_top, godot_real p_near, godot_real p_far) {
    godot_real x = 2 * p_near / (p_right - p_left);
    godot_real y = 2 * p_near / (p_top - p_bottom);

    godot_real a = (p_right + p_left) / (p_right - p_left);
    godot_real b = (p_top + p_bottom) / (p_top - p_bottom);
    godot_real c = -(p_far + p_near) / (p_far - p_near);
    godot_real d = -2 * p_far * p_near / (p_far - p_near);

    // Godot is column major
    p_projection[0] = x;
    p_projection[1] = 0;
    p_projection[2] = 0;
    p_projection[3] = 0;
    p_projection[4] = 0;
    p_projection[5] = y;
    p_projection[6] = 0;
    p_projection[7] = 0;
    p_projection[8] = a;
    p_projection[9] = b;
    p_projection[10] = c;
    p_projection[11] = -1;
    p_projection[12] = 0;
    p_projection[13] = 0;
    p_projection[14] = d;
    p_projection[15] = 0;
}

void printCam(godot_real *m) {
    printf("%f %f %f %f\n", m[0],m[4], m[8], m[12]);
    printf("%f %f %f %f\n", m[1],m[5], m[9], m[13]);
    printf("%f %f %f %f\n", m[2],m[6], m[10], m[14]);
    printf("%f %f %f %f\n", m[3],m[7], m[11], m[15]);
}


godot_transform camToTransform(godot_real *p_projection) {
    // Rotate the projection to be non-perpendicular.
    godot_basis basis;
    api->godot_basis_new(&basis);
    godot_vector3 r1, r2, r3, off;

    // get current projection matrix
    api->godot_vector3_new(&r1, p_projection[0], p_projection[1], p_projection[2]);
    api->godot_vector3_new(&r2, p_projection[4], p_projection[5], p_projection[6]);
    api->godot_vector3_new(&r3, p_projection[8], p_projection[9], p_projection[10]);
    // offset
    api->godot_vector3_new(&off, p_projection[12],p_projection[13],p_projection[14]);

    api->godot_basis_set_row(&basis, 0, &r1);
    api->godot_basis_set_row(&basis, 1, &r2);
    api->godot_basis_set_row(&basis, 2, &r3);

    godot_transform M;
    api->godot_transform_new(&M, &basis, &off);

    return M;
}

void TransformToCam(const godot_transform& in, godot_real *p_projection) {
    godot_basis basis;
    api->godot_basis_new(&basis);
    godot_vector3 r1, r2, r3, off;
    api->godot_vector3_new(&off, 0, 0, 0);

    basis = api->godot_transform_get_basis(&in);
    r1 = api->godot_basis_get_row(&basis, 0);
    r2 = api->godot_basis_get_row(&basis, 1);
    r3 = api->godot_basis_get_row(&basis, 2);
    off = api->godot_transform_get_origin(&in);

    p_projection[0] = api->godot_vector3_get_axis(&r1, godot_vector3_axis::GODOT_VECTOR3_AXIS_X);
    p_projection[1] = api->godot_vector3_get_axis(&r1, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y);
    p_projection[2] = api->godot_vector3_get_axis(&r1, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z);
    p_projection[3] = 0;

    p_projection[4] = api->godot_vector3_get_axis(&r2, godot_vector3_axis::GODOT_VECTOR3_AXIS_X);
    p_projection[5] = api->godot_vector3_get_axis(&r2, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y);
    p_projection[6] = api->godot_vector3_get_axis(&r2, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z);
    p_projection[7] = 0;

    p_projection[8] = api->godot_vector3_get_axis(&r3, godot_vector3_axis::GODOT_VECTOR3_AXIS_X);
    p_projection[9] = api->godot_vector3_get_axis(&r3, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y);
    p_projection[10] = api->godot_vector3_get_axis(&r3, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z);
    p_projection[11] = -1;

    p_projection[12] = api->godot_vector3_get_axis(&off, godot_vector3_axis::GODOT_VECTOR3_AXIS_X);
    p_projection[13] = api->godot_vector3_get_axis(&off, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y);
    p_projection[14] = api->godot_vector3_get_axis(&off, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z);
    p_projection[15] = 0;
}

void godot_arvr_fill_projection_for_eye(void *p_data, godot_real *p_projection,
                                        godot_int p_eye, godot_real p_aspect,
                                        godot_real p_z_near,
                                        godot_real p_z_far) {
    auto *arvr_data = (arvr_data_struct *)p_data;
    assert(arvr_data != nullptr); // "Invalid arvr data"

    // arbitrary projection from: https://csc.lsu.edu/~kooima/articles/genperspective/
    float n = p_z_near;
    float f = p_z_far;

    godot_vector3 pe = get_eye_pos(p_data, p_eye);
    //printVector(&pe);

    // Compute an orthonormal basis for the screen.
    arvr_data->vr = api->godot_vector3_operator_subtract(&arvr_data->pb, &arvr_data->pa);
    arvr_data->vu = api->godot_vector3_operator_subtract(&arvr_data->pc, &arvr_data->pa);

    arvr_data->vr = api->godot_vector3_normalized(&arvr_data->vr);
    arvr_data->vu = api->godot_vector3_normalized(&arvr_data->vu);

    arvr_data->vn = api->godot_vector3_cross(&arvr_data->vr, &arvr_data->vu);
    arvr_data->vn = api->godot_vector3_normalized(&arvr_data->vn);

    // Compute the screen corner vectors.
    arvr_data->va = api->godot_vector3_operator_subtract(&arvr_data->pa, &pe);
    arvr_data->vb = api->godot_vector3_operator_subtract(&arvr_data->pb, &pe);
    arvr_data->vc = api->godot_vector3_operator_subtract(&arvr_data->pc, &pe);

    // Find the distance from the eye to screen plane.
    float d = - api->godot_vector3_dot(&arvr_data->vn, &arvr_data->va);

    // Find the extent of the perpendicular projection.
    float l = api->godot_vector3_dot(&arvr_data->vr, &arvr_data->va) * n / d;
    float r = api->godot_vector3_dot(&arvr_data->vr, &arvr_data->vb) * n / d;
    float b = api->godot_vector3_dot(&arvr_data->vu, &arvr_data->va) * n / d;
    float t = api->godot_vector3_dot(&arvr_data->vu, &arvr_data->vc) * n / d;

    // Load the perpendicular projection.
    arvr_set_frustum(p_projection, l, r, b, t, n, f);

    // Rotate the projection to be non-perpendicular. (needed for projection planes not alignet with the XY plane)
    godot_basis basis;
    api->godot_basis_new(&basis);

    // transposed rotation matrix
    api->godot_basis_set_row(&basis, 0, &arvr_data->vr);
    api->godot_basis_set_row(&basis, 1, &arvr_data->vu);
    api->godot_basis_set_row(&basis, 2, &arvr_data->vn);

    godot_vector3 off;
    api->godot_vector3_new(&off, 0,0,0);
    off = api->godot_vector3_operator_subtract(&off, &pe); // use the position here or in the get eye pos function
    godot_transform Mt;
    api->godot_transform_new(&Mt, &basis, &off);

    godot_transform P = camToTransform(p_projection);

    // this does not do anything if the screen is aligned with the coordinate system
    // vr, vu and vn will be [1,0,0], [0,1,0], [0,0,1]
    P = api->godot_transform_operator_multiply(&Mt, &P);

    // back to opengl
    TransformToCam(P, p_projection);

    godot_vector3 offset;
    godot_basis head_rotation;
    godot_real world_scale = 1;//arvr_api->godot_arvr_get_worldscale();
    api->godot_basis_new_with_euler_quat(&head_rotation, &arvr_data->re);
    if (p_eye == 1) {
        // left eye
        api->godot_vector3_new(&offset, -arvr_data->iod_m * 0.5 * world_scale, 0.0, 0.0);
        offset = api->godot_basis_xform(&head_rotation, &offset);
    } else if (p_eye == 2) {
        // right eye
        api->godot_vector3_new(&offset, arvr_data->iod_m * 0.5 * world_scale, 0.0, 0.0);
        offset = api->godot_basis_xform(&head_rotation, &offset);
    }
    arvr_api->godot_pw_set_offset(&offset);
}

godot_int godot_arvr_get_external_texture_for_eye(void *p_data, godot_int p_eye) {
    // we don't support this
    return 0;
}

void godot_arvr_commit_for_eye(void *p_data, godot_int p_eye,
                               godot_rid *p_render_target,
                               godot_rect2 *p_screen_rect) {
    auto *arvr_data = (arvr_data_struct *)p_data;
    assert(arvr_data != nullptr); // "Invalid arvr data.");
    assert(p_eye != /* EYE_MONO */ 0); // "Mono rendering is not supported.");
    if (arvr_data->swap_eyes)
        p_eye = p_eye == 1 ? 2 : 1;
    arvr_api->godot_arvr_blit(p_eye, p_render_target, p_screen_rect);
}

void godot_arvr_process(void *p_data) {
    auto *arvr_data = (arvr_data_struct *)p_data;
    assert(arvr_data != nullptr); // "Invalid arvr data.");

    if (arvr_data->vrpnTracker) {
        arvr_data->vrpnTracker->mainloop();
    }

    updateOpentrack(arvr_data);

    arvr_api->godot_pw_set_mode(arvr_data->enable_edge_adjust);
    arvr_api->godot_pw_set_pa(&arvr_data->pa);
    arvr_api->godot_pw_set_pb(&arvr_data->pb);
    arvr_api->godot_pw_set_pc(&arvr_data->pc);
    arvr_api->godot_pw_set_pe(&arvr_data->pe);

    // TODO fix this

//    godot::SceneTree* tree  = (godot::SceneTree*)godot::Engine::get_singleton()->get_main_loop();
//    auto a = tree->get_root();
//
//    //godot::String originPath = "Spatial/robot/camera_base/camera_rot/Camera/ARVROrigin";
//    godot::String originPath = "Main/ARVROrigin";
//    auto camera = ((godot::Spatial*)a->get_node(originPath + "/ARVRCamera"));
//    arvr_data_struct * arvr_data = (arvr_data_struct *) p_data;
//    arvr_data->pe = camera->get_translation();
//    auto r = camera->get_rotation();
//    api->godot_vector3_set_axis(&arvr_data->re, godot_vector3_axis::GODOT_VECTOR3_AXIS_X, r.x);
//    api->godot_vector3_set_axis(&arvr_data->re, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y, r.y);
//    api->godot_vector3_set_axis(&arvr_data->re, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z, r.z);
//
//    auto pa = ((godot::Spatial*)a->get_node(originPath + "/Screen/pa"));
//    auto pb = ((godot::Spatial*)a->get_node(originPath + "/Screen/pb"));
//    auto pc = ((godot::Spatial*)a->get_node(originPath + "/Screen/pc"));
//    arvr_data->pa = pa->get_translation();
//    arvr_data->pb = pb->get_translation();
//    arvr_data->pc = pc->get_translation();
}

// set_euler_yxz expects a vector containing the Euler angles in the format
// (ax,ay,az), where ax is the angle of rotation around x axis,
// and similar for other axes.
// This implementation uses YXZ convention (Z is the first rotation).
void quat_set_euler_yxz(godot_quat *quat, const godot_vector3 &p_euler) {
    float half_a1 = api->godot_vector3_get_axis(&p_euler, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y) * 0.5;
    float half_a2 = api->godot_vector3_get_axis(&p_euler, godot_vector3_axis::GODOT_VECTOR3_AXIS_X) * 0.5;
    float half_a3 = api->godot_vector3_get_axis(&p_euler, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z) * 0.5;

    // R = Y(a1).X(a2).Z(a3) convention for Euler angles.
    // Conversion to quaternion as listed in https://ntrs.nasa.gov/archive/nasa/casi.ntrs.nasa.gov/19770024290.pdf (page A-6)
    // a3 is the angle of the first rotation, following the notation in this reference.

    float cos_a1 = cos(half_a1);
    float sin_a1 = sin(half_a1);
    float cos_a2 = cos(half_a2);
    float sin_a2 = sin(half_a2);
    float cos_a3 = cos(half_a3);
    float sin_a3 = sin(half_a3);

    api->godot_quat_set_x(quat, sin_a1 * cos_a2 * sin_a3 + cos_a1 * sin_a2 * cos_a3);
    api->godot_quat_set_y(quat, sin_a1 * cos_a2 * cos_a3 - cos_a1 * sin_a2 * sin_a3);
    api->godot_quat_set_z(quat, -sin_a1 * sin_a2 * cos_a3 + cos_a1 * cos_a2 * sin_a3);
    api->godot_quat_set_w(quat, sin_a1 * sin_a2 * sin_a3 + cos_a1 * cos_a2 * cos_a3);
}

void updateOpentrack(arvr_data_struct *arvr_data) {
    if (arvr_data->opentrack->update()) {
        // translation
        float x = static_cast<float>(-arvr_data->opentrack->currentPose[0]) / 100.0f;
        float y = static_cast<float>(arvr_data->opentrack->currentPose[1]) / 100.0f;
        float z = static_cast<float>(arvr_data->opentrack->currentPose[2]) / 100.0f;
        api->godot_vector3_set_axis(&arvr_data->pe, godot_vector3_axis::GODOT_VECTOR3_AXIS_X, x);
        api->godot_vector3_set_axis(&arvr_data->pe, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y, y);
        api->godot_vector3_set_axis(&arvr_data->pe, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z, z);

        // rotation
//        godot_vector3 angles;
//        api->godot_vector3_new(&angles, static_cast<float>(arvr_data->opentrack->currentPose[3]),
//                static_cast<float>(arvr_data->opentrack->currentPose[4]),
//                static_cast<float>(arvr_data->opentrack->currentPose[5]));
//        quat_set_euler_yxz(&arvr_data->re, angles);
    }
}

void *godot_arvr_constructor(godot_object *p_instance) {
    printf("PowerwallInterface godot_arvr_constructor\n");
    arvr_data = (arvr_data_struct *)api->godot_alloc(sizeof(arvr_data_struct));
    arvr_data->instance = p_instance;
    arvr_data->is_initialised = false;
    // *x for debugging
    arvr_data->iod_m = 5.5 / 100.0;
    arvr_data->enable_edge_adjust = 0;
    arvr_data->vrpnTracker = nullptr;

    // projection screen coordinates - these are updated in the arvr-process method
//    api->godot_vector3_new(&arvr_data->pa, -2, 0.0,  0);
//    api->godot_vector3_new(&arvr_data->pb,  2, 0.0, -0);
//    api->godot_vector3_new(&arvr_data->pc, -2, 2.5, -0);

    // monitor at home
    api->godot_vector3_new(&arvr_data->pa, -.41, -0.45,  0); // buttom left
    api->godot_vector3_new(&arvr_data->pb,  .41, -0.45, -0); // buttom right
    api->godot_vector3_new(&arvr_data->pc, -.41, 0., -0);  // upper left


    // eye coordinates - updated in the arvr-process method
    api->godot_vector3_new(&arvr_data->pe, 0,0,0);
    api->godot_quat_new(&arvr_data->re, 0,0,0,1);

    return arvr_data;
};

void godot_arvr_destructor(void *p_data) {
    if (p_data != nullptr) {
        auto *arvr_data = (arvr_data_struct *)p_data;

        if (arvr_data->vrpnTracker) {
            delete arvr_data->vrpnTracker;
        }

        delete arvr_data->opentrack;

        api->godot_free(p_data);
    }
}
