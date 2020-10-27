#include <climits>
////////////////////////////////////////////////////////////////////////////
// This is just an example POWERWALL GDNative module. It does very little VR-ish
// It also outputs lots of debugging stuff which should not be in a proper
// module :)

// Written by Bastiaan "Mux213" Olij, with loads of help from Thomas "Karroffel" Herzog

#include "PowerwallInterface.h"
#include <cassert>
#include <iostream>
#include <cmath>
#include "utils.h"

arvr_data_struct *g_arvr_data = nullptr;

void updateOpentrack(arvr_data_struct *arvr_data);

void updatePowerwallCoords();

namespace {
const char *kName = "Powerwall";
} // namespace

// Eye position in world space
godot_vector3 _get_eye_pos(void *p_data, int p_eye) {
    auto *arvr_data = (arvr_data_struct *)p_data;
    assert(arvr_data != nullptr); // Invalid arvr data

    godot_transform eye_offset_transform;
    godot_transform hmd_transform;

    // Currently we only support 1to1 scaling.
    godot_real world_scale = 1;//arvr_api->godot_arvr_get_worldscale();

    godot_basis head_rotation;
    //api->godot_basis_new_with_euler(&head_rotation, &g_arvr_data->re);
    api->godot_basis_new_with_euler_quat(&head_rotation, &arvr_data->re);

    // create our transform from head center to eye
    api->godot_transform_new_identity(&eye_offset_transform);
    if (p_eye == 1) {
        // left eye
        godot_vector3 offset;
        api->godot_vector3_new(&offset, -arvr_data->iod_m * 0.5 * world_scale, 0.0, 0.0);
        offset = api->godot_basis_xform(&head_rotation, &offset);
//        if (rand() % 60 == 0) {
//            printf("Offset for eye %d: ", p_eye);
//            printVector(&offset);
//        }
        eye_offset_transform = api->godot_transform_translated(&eye_offset_transform, &offset);
    } else if (p_eye == 2) {
        // right eye
        godot_vector3 offset;
        api->godot_vector3_new(&offset, arvr_data->iod_m * 0.5 * world_scale, 0.0, 0.0);
        offset = api->godot_basis_xform(&head_rotation, &offset);
        eye_offset_transform = api->godot_transform_translated(&eye_offset_transform, &offset);
    } else {
        // leave in the middle, mono
    }

    // now determine our HMD positional tracking
    api->godot_transform_new_identity(&hmd_transform);
    hmd_transform = api->godot_transform_translated(&hmd_transform, &arvr_data->pe);

    // Now construct our full transform
    godot_transform reference_frame = arvr_api->godot_arvr_get_reference_frame();

    godot_transform transform; // = g_arvr_data->godot_cam_transform;
    api->godot_transform_new_identity(&transform);

    //transform = api->godot_transform_operator_multiply(&transform, &reference_frame);
    transform = api->godot_transform_operator_multiply(&transform, &hmd_transform);
    transform = api->godot_transform_operator_multiply(&transform, &eye_offset_transform);

//    transform = api->godot_transform_operator_multiply(&reference_frame, &transform);
//    transform = api->godot_transform_operator_multiply(&hmd_transform, &transform);
//    transform = api->godot_transform_operator_multiply(&eye_offset_transform, &transform);

    godot_vector3 ret;
    api->godot_vector3_new(&ret,0,0,0);
    ret = api->godot_transform_xform_vector3(&transform, &ret);
    if (rand() % 60 == 0) {
        printf("Eye pos for eye %d: ", p_eye);
        printVector(&ret);
    }
    return ret;
}

// reimplementation of openGL projection matrix junction
void _arvr_set_frustum(godot_real *p_projection, godot_real p_left, godot_real p_right, godot_real p_bottom, godot_real p_top, godot_real p_near, godot_real p_far) {
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

// Create a godot transform object from an OpenGL camera matrix
godot_transform _camToTransform(godot_real *p_projection) {
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


///////////////////
// ARVR interface implementation
///////////////////

godot_string godot_arvr_get_name(__unused const void *p_data) {
    godot_string ret;

    printf("Powerwall interface get name.\n");

    api->godot_string_new(&ret);
    api->godot_string_parse_utf8(&ret, kName);

    return ret;
}

godot_int godot_arvr_get_capabilities(__unused const void *p_data) {
    printf("Powerwall interface get capabilities.");
    return 2; // 2 = ARVR_STEREO
}

godot_bool godot_arvr_get_anchor_detection_is_enabled(__unused const void *p_data) {
    // does not apply here
    return false;
}

void godot_arvr_set_anchor_detection_is_enabled(__unused void *p_data, __unused bool p_enable) {
    // we ignore this, not supported in this interface!
}

void godot_arvr_notification(__unused void *p_data, __unused godot_int p_what) {
    // we don't care
}

godot_bool godot_arvr_is_stereo(__unused const void *p_data) {
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

        api->godot_string_new(&arvr_data->tracker_url);
        arvr_data->vrpnTracker = nullptr;

        arvr_data->opentrack = new OpentrackServer(4242);

        arvr_data->swap_eyes = true;

        arvr_data->is_initialised = true;
    }

    // and return our result
    ret = arvr_data->is_initialised;
    return ret;
}

void godot_arvr_uninitialize(void *p_data) {
    auto * arvr_data = (arvr_data_struct *) p_data;

    if (arvr_data->is_initialised) {
        // cleanup if needed
        arvr_data->is_initialised = false;
    }
}

/* returns the recommended render target size per eye for this device */
godot_vector2 godot_arvr_get_render_targetsize(const void *p_data) {
    auto *arvr_data = (arvr_data_struct *)p_data;
    assert(arvr_data != nullptr); // Invalid arvr data

    godot_vector2 size;
    if (arvr_data->home_debug) {
        api->godot_vector2_new(&size, 2560, 1600);
    } else {
        api->godot_vector2_new(&size, 1920, 1080);
    }
    return size;
}

godot_transform godot_arvr_get_transform_for_eye(void *p_data, __unused godot_int p_eye, godot_transform *p_cam_transform) {
    // BUG: The code in godot looks like we should only return the offset to the cyclone's eye -- but nobody does that
    // in the godot VR plugins.
    // The result will be used for culling
    auto *arvr_data = (arvr_data_struct *)p_data;
    assert(arvr_data != nullptr); // Invalid arvr data

    // save the last camera transform we got from godot
    arvr_data->godot_cam_transform = *p_cam_transform;
//    { // debug that cam transform is identity
//        float projectionMatrix[16];
//        TransformToCam(*p_cam_transform, projectionMatrix);
//        printCam(projectionMatrix);
//    }

    godot_transform ret;
    api->godot_transform_new_identity(&ret);
    return ret;

//    godot_transform reference_frame = arvr_api->godot_arvr_get_reference_frame();
//
//    godot_transform transform_for_eye;
//    godot_transform hmd_transform;
//    godot_vector3 offset;
//    // Currently we only support 1to1 scaling.
//    godot_real world_scale = 1;//arvr_api->godot_arvr_get_worldscale();
//
//    godot_basis head_rotation;
//    api->godot_basis_new_with_euler_quat(&head_rotation, &arvr_data->re);
//
//
//    // create our transform from head center to eye
//    api->godot_transform_new_identity(&transform_for_eye);
//    if (p_eye == 1) {
//        // left eye
//        api->godot_vector3_new(&offset, -arvr_data->iod_m * 0.5 * world_scale, 0.0, 0.0);
//        offset = api->godot_basis_xform(&head_rotation, &offset);
//        transform_for_eye = api->godot_transform_translated(&transform_for_eye, &offset);
//    } else if (p_eye == 2) {
//        // right eye
//        api->godot_vector3_new(&offset, arvr_data->iod_m * 0.5 * world_scale, 0.0, 0.0);
//        offset = api->godot_basis_xform(&head_rotation, &offset);
//        transform_for_eye = api->godot_transform_translated(&transform_for_eye, &offset);
//    } else {
//        // leave in the middle, mono
//    }
//
//    // now determine our HMD positional tracking
//    api->godot_transform_new_identity(&hmd_transform);
//    hmd_transform = api->godot_transform_translated(&hmd_transform, &arvr_data->pe);
//
//    // Now construct our full transform, the order may be in reverse, have to test :)
//    // ignore cam and reference here to be compatible to the shader
//    //ret = *p_cam_transform;
//    //ret = api->godot_transform_operator_multiply(&ret, &reference_frame);
//
//    //ret = api->godot_transform_operator_multiply(&ret, &hmd_transform);
//    //ret = api->godot_transform_operator_multiply(&ret, &transform_for_eye);
//
//    return ret; // transform; //ret;
}


void godot_arvr_fill_projection_for_eye(void *p_data, godot_real *p_projection,
                                        godot_int p_eye, __unused godot_real p_aspect,
                                        godot_real p_z_near,
                                        godot_real p_z_far) {
    auto *arvr_data = (arvr_data_struct *)p_data;
    assert(arvr_data != nullptr); // "Invalid arvr data"

    // arbitrary projection from: https://csc.lsu.edu/~kooima/articles/genperspective/
    float n = p_z_near;
    float f = p_z_far;

    godot_vector3 pe = _get_eye_pos(p_data, p_eye);
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
    _arvr_set_frustum(p_projection, l, r, b, t, n, f);

    // Rotate the projection to be non-perpendicular. (needed for projection planes not alignet with the XY plane)
    // This aligns the projection canvas plane with the OpenGL xy-plane
    // transposed rotation matrix M^T
    godot_basis basis;
    api->godot_basis_new(&basis);

//    {
//        godot_vector3 t1, t2, t3;
//        float rx = api->godot_vector3_get_axis(&g_arvr_data->vr, GODOT_VECTOR3_AXIS_X);
//        float ry = api->godot_vector3_get_axis(&g_arvr_data->vr, GODOT_VECTOR3_AXIS_Y);
//        float rz = api->godot_vector3_get_axis(&g_arvr_data->vr, GODOT_VECTOR3_AXIS_Z);
//        float ux = api->godot_vector3_get_axis(&g_arvr_data->vu, GODOT_VECTOR3_AXIS_X);
//        float uy = api->godot_vector3_get_axis(&g_arvr_data->vu, GODOT_VECTOR3_AXIS_Y);
//        float uz = api->godot_vector3_get_axis(&g_arvr_data->vu, GODOT_VECTOR3_AXIS_Z);
//        float nx = api->godot_vector3_get_axis(&g_arvr_data->vn, GODOT_VECTOR3_AXIS_X);
//        float ny = api->godot_vector3_get_axis(&g_arvr_data->vn, GODOT_VECTOR3_AXIS_Y);
//        float nz = api->godot_vector3_get_axis(&g_arvr_data->vn, GODOT_VECTOR3_AXIS_Z);
//        api->godot_vector3_new(&t1, rx, ux, nx);
//        api->godot_vector3_new(&t2, ry, uy, ny);
//        api->godot_vector3_new(&t3, rz, uz, nz);
//        api->godot_basis_new_with_rows(&basis, &t1, &t2, &t3);
//    }
    api->godot_basis_new_with_rows(&basis, &arvr_data->vr, &arvr_data->vu, &arvr_data->vn);

    godot_vector3 off;
    api->godot_vector3_new(&off, 0,0,0);
    //off = api->godot_vector3_operator_subtract(&off, &pe); // use the position here or in the get eye pos function
    //off = api->godot_vector3_operator_add(&off, &pe); // use the position here or in the get eye pos function
    godot_transform Mt;
    api->godot_transform_new(&Mt, &basis, &off);

    godot_transform P = _camToTransform(p_projection);

    // set eye offset for the shader - we don't ned to use it for the matrix calculation
    // as this is already included in our eye position we work with
    {
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
        } else {
            // eye 0 for culling
            api->godot_vector3_new(&offset, 0,0,0);
        }
        // the offset has it's origin in the cyclope's eye
        arvr_api->godot_pw_set_offset(&offset);
    }

    godot_transform transform_for_eye;
    api->godot_transform_new_identity(&transform_for_eye);
    pe = api->godot_vector3_operator_multiply_scalar(&pe, -1);
    transform_for_eye = api->godot_transform_translated(&transform_for_eye, &pe);

    godot_transform PMT;
    api->godot_transform_new_identity(&PMT);
    PMT = api->godot_transform_operator_multiply(&P, &PMT);
    PMT = api->godot_transform_operator_multiply(&Mt, &PMT);
    PMT = api->godot_transform_operator_multiply(&transform_for_eye, &PMT);

//    PMT = api->godot_transform_operator_multiply(&PMT, &P);
//    PMT = api->godot_transform_operator_multiply(&PMT, &transform_for_eye);
//    PMT = api->godot_transform_operator_multiply(&PMT, &Mt);


    // back to opengl
    TransformToCam(PMT, p_projection);
    //TransformToCam(P, p_projection);
}

godot_int godot_arvr_get_external_texture_for_eye(__unused void *p_data, __unused godot_int p_eye) {
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

    if (arvr_data->home_debug){
        //printf("eye %d rect: ", p_eye);
        //printRect(p_screen_rect);
        godot_vector2 s = api->godot_rect2_get_size(p_screen_rect);
        godot_vector2 p = api->godot_rect2_get_position(p_screen_rect);
        int windowWidth = 2560*2;
        api->godot_vector2_set_x(&s, windowWidth*2);
        if (p_eye == 2) {
            api->godot_vector2_set_x(&p, -windowWidth);
        }
        api->godot_rect2_set_size(p_screen_rect, &s);
        api->godot_rect2_set_position(p_screen_rect, &p);
        arvr_api->godot_arvr_blit(p_eye, p_render_target, p_screen_rect);
    } else {
        arvr_api->godot_arvr_blit(p_eye, p_render_target, p_screen_rect);
    }
}

void godot_arvr_process(void *p_data) {
    auto *arvr_data = (arvr_data_struct *)p_data;
    assert(arvr_data != nullptr); // "Invalid arvr data.");

    updatePowerwallCoords();

    if (arvr_data->vrpnTracker) {
        arvr_data->vrpnTracker->mainloop();
    }

    updateOpentrack(arvr_data);

    arvr_api->godot_pw_set_mode(arvr_data->enable_edge_adjust);
    arvr_api->godot_pw_set_pa(&arvr_data->pa);
    arvr_api->godot_pw_set_pb(&arvr_data->pb);
    arvr_api->godot_pw_set_pc(&arvr_data->pc);
    arvr_api->godot_pw_set_pe(&arvr_data->pe);
}

/**
 * Update projection plane coordinates from scene
 */
void updatePowerwallCoords() {// TODO fix this (interactive monitor placement with objects from the world)
//    godot::SceneTree* tree  = (godot::SceneTree*)godot::Engine::get_singleton()->get_main_loop();
//    auto a = tree->get_root();
//
//    //godot::String originPath = "Spatial/robot/camera_base/camera_rot/Camera/ARVROrigin";
//    godot::String originPath = "Main/ARVROrigin";
//    auto camera = ((godot::Spatial*)a->get_node(originPath + "/ARVRCamera"));
//    arvr_data_struct * g_arvr_data = (arvr_data_struct *) p_data;
//    g_arvr_data->pe = camera->get_translation();
//    auto r = camera->get_rotation();
//    api->godot_vector3_set_axis(&g_arvr_data->re, godot_vector3_axis::GODOT_VECTOR3_AXIS_X, r.x);
//    api->godot_vector3_set_axis(&g_arvr_data->re, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y, r.y);
//    api->godot_vector3_set_axis(&g_arvr_data->re, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z, r.z);
//
//    auto pa = ((godot::Spatial*)a->get_node(originPath + "/Screen/pa"));
//    auto pb = ((godot::Spatial*)a->get_node(originPath + "/Screen/pb"));
//    auto pc = ((godot::Spatial*)a->get_node(originPath + "/Screen/pc"));
//    g_arvr_data->pa = pa->get_translation();
//    g_arvr_data->pb = pb->get_translation();
//    g_arvr_data->pc = pc->get_translation();
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
//        api->godot_vector3_new(&angles, static_cast<float>(g_arvr_data->opentrack->currentPose[3]),
//                static_cast<float>(g_arvr_data->opentrack->currentPose[4]),
//                static_cast<float>(g_arvr_data->opentrack->currentPose[5]));
//        quat_set_euler_yxz(&g_arvr_data->re, angles);
    }
}

void *godot_arvr_constructor(godot_object *p_instance) {
    printf("PowerwallInterface godot_arvr_constructor\n");
    g_arvr_data = (arvr_data_struct *)api->godot_alloc(sizeof(arvr_data_struct));
    g_arvr_data->instance = p_instance;
    g_arvr_data->is_initialised = false;
    // *x for debugging
    g_arvr_data->iod_m = 6.5 / 100.0;
    g_arvr_data->enable_edge_adjust = 0;
    g_arvr_data->vrpnTracker = nullptr;
    api->godot_transform_new_identity(&g_arvr_data->godot_cam_transform);

    // debugging settings
    g_arvr_data->home_debug = true;

    // projection screen coordinates - these are updated in updatePowerwallCoords method -- which currently is not implemented
    // world coordinates
    if (g_arvr_data->home_debug) {
        // monitor at home -- tracking center is horizontally centered at the upper edge
        api->godot_vector3_new(&g_arvr_data->pa, -.41, -0.45, 0); // buttom left
        api->godot_vector3_new(&g_arvr_data->pb, .41, -0.45, -0); // buttom right
        api->godot_vector3_new(&g_arvr_data->pc, -.41, 0.0, -0);  // upper left
    } else {
        api->godot_vector3_new(&g_arvr_data->pa, -2, 0.0, 0);
        api->godot_vector3_new(&g_arvr_data->pb,  2, 0.0, 0);
        api->godot_vector3_new(&g_arvr_data->pc, -2, 2.5, 0);
    }

    // eye coordinates - updated in the arvr-process method
    // world coordinates
    api->godot_vector3_new(&g_arvr_data->pe, 0, 0, 0.7); // will be overwritten by tracking
    //api->godot_vector3_new(&g_arvr_data->pe, 0, 1.75,2); // Powerwall
    api->godot_quat_new(&g_arvr_data->re, 0, 0, 0, 1);

    return g_arvr_data;
}

void godot_arvr_destructor(void *p_data) {
    if (p_data != nullptr) {
        auto *arvr_data = (arvr_data_struct *)p_data;

        delete arvr_data->vrpnTracker;
        delete arvr_data->opentrack;

        api->godot_free(p_data);
    }
}
