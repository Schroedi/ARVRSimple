////////////////////////////////////////////////////////////////////////////
// This is just an example ARVR GDNative module. It does very little VR-ish
// It also outputs lots of debugging stuff which should not be in a proper
// module :)

// Written by Bastiaan "Mux213" Olij, with loads of help from Thomas "Karroffel" Herzog

#include "ARVRInterface.h"
#include "ARVRScreen.h"
#include <Godot.hpp>
#include <Node.hpp>
#include <Spatial.hpp>
#include <SceneTree.hpp>
#include <Engine.hpp>
#include <Viewport.hpp>
#include <cassert>
#include "String.hpp"

//using namespace godot;

void *godot_arvr_constructor(godot_object *p_instance) {
	godot_string ret;

	arvr_data_struct *arvr_data = (arvr_data_struct *)api->godot_alloc(sizeof(arvr_data_struct));
	arvr_data->instance = p_instance;
	arvr_data->is_initialised = false;
	arvr_data->iod_cm = 6.1;
	arvr_data->oversample = 1.0;

    // projection screen coordinates
    arvr_data->pa = godot::Vector3(-2, 0, 0);
    arvr_data->pb = godot::Vector3( 2, 0, -0);
    arvr_data->pc = godot::Vector3(-2, 2.5, -0);
    // arvr_data->pa = godot::Vector3(-2, -1.3, 0);
    // arvr_data->pb = godot::Vector3( 2, -1.3, 0);
    // arvr_data->pc = godot::Vector3(-2, 2.5, 0);
    
    // eye coordinates
    arvr_data->pe = godot::Vector3();
    api->godot_vector3_new(&arvr_data->re, 0,0,0);

	return arvr_data;
}

void godot_arvr_destructor(void *p_data) {
	if (p_data != NULL) {
		api->godot_free(p_data);
	}
}

godot_string godot_arvr_get_name(const void *p_data) {
	godot_string ret;

	char name[] = "ARVRSimple";
	api->godot_string_new(&ret);
	api->godot_string_parse_utf8(&ret, name);

	return ret;
}

godot_int godot_arvr_get_capabilities(const void *p_data) {
	godot_int ret;

	ret = 2; // 2 = ARVR_STEREO

	return ret;
}

godot_bool godot_arvr_get_anchor_detection_is_enabled(const void *p_data) {
	godot_bool ret;

	ret = false; // does not apply here

	return ret;
}

void godot_arvr_set_anchor_detection_is_enabled(void *p_data, bool p_enable) {
	// we ignore this, not supported in this interface!
}

godot_bool godot_arvr_is_stereo(const void *p_data) {
	godot_bool ret;

	// printf("ARVRSimple.arvr_is_stereo()\n");
	ret = true;

	return ret;
}

godot_bool godot_arvr_is_initialized(const void *p_data) {
	godot_bool ret;
	arvr_data_struct *arvr_data = (arvr_data_struct *)p_data;

	ret = arvr_data == NULL ? false : arvr_data->is_initialised;

	return ret;
}
 
godot_bool godot_arvr_initialize(void *p_data) {
	godot_bool ret;
	arvr_data_struct * arvr_data = (arvr_data_struct *) p_data;

	if (!arvr_data->is_initialised) {
		// initialise this interface, so initialize any 3rd party libraries, open up HMD window if required, etc.


		// note, this will be made the primary interface by ARVRInterfaceGDNative
		arvr_data->is_initialised = true;
	}

	// and return our result
	ret = arvr_data->is_initialised;
	return ret;
}

void godot_arvr_uninitialize(void *p_data) {
	arvr_data_struct * arvr_data = (arvr_data_struct *) p_data;

	if (arvr_data->is_initialised) {
		// note, this will already be removed as the primary interface by ARVRInterfaceGDNative

		// cleanup if needed

		arvr_data->is_initialised = false;
	}
}

godot_vector2 godot_arvr_get_recommended_render_targetsize(const void *p_data) {
	godot_vector2 size;

	// printf("ARVRSimple.arvr_get_recommended_render_targetsize()\n");

	api->godot_vector2_new(&size, 5120, 1600);

	return size;
}

godot_transform godot_arvr_get_transform_for_eye(void *p_data, godot_int p_eye, godot_transform *p_cam_transform) {
	arvr_data_struct * arvr_data = (arvr_data_struct *) p_data;
	godot_transform transform_for_eye;
	godot_transform hmd_transform;
	godot_transform reference_frame = arvr_api->godot_arvr_get_reference_frame();
	godot_transform ret;
	godot_vector3 offset;
	godot_real world_scale = arvr_api->godot_arvr_get_worldscale();

    godot_basis head_rotation;
    api->godot_basis_new_with_euler(&head_rotation, &arvr_data->re);
    // auto r = api->godot_basis_get_euler(&head_rotation);
    // printf("rot: %f", godot_vector3_get_axis(&r, godot_vector3_axis::GODOT_VECTOR3_AXIS_X));
    
	// create our transform from head center to eye
	api->godot_transform_new_identity(&transform_for_eye);
	if (p_eye == 1) {
		// left eye
		api->godot_vector3_new(&offset, -arvr_data->iod_cm * 0.01 * 0.5 * world_scale, 0.0, 0.0);
        offset = api->godot_basis_xform(&head_rotation, &offset);
		transform_for_eye = api->godot_transform_translated(&transform_for_eye, &offset);
	} else if (p_eye == 2) {
		// right eye
		api->godot_vector3_new(&offset, arvr_data->iod_cm * 0.01 * 0.5 * world_scale, 0.0, 0.0);
        offset = api->godot_basis_xform(&head_rotation, &offset);
		transform_for_eye = api->godot_transform_translated(&transform_for_eye, &offset);
	} else {
		// leave in the middle, mono
	}

	// // now determine our HMD positional tracking, we have none in this interface...
	api->godot_transform_new_identity(&hmd_transform);
	api->godot_vector3_new(&offset, arvr_data->pe.x, arvr_data->pe.y, arvr_data->pe.z);
	hmd_transform = api->godot_transform_translated(&hmd_transform, &offset);

	// Now construct our full transform, the order may be in reverse, have to test :)
	ret = *p_cam_transform;
    //api->godot_transform_new_identity(&ret);
    //ret = api->godot_transform_operator_multiply(&ret, p_cam_transform);
	ret = api->godot_transform_operator_multiply(&ret, &reference_frame);
    ret = api->godot_transform_operator_multiply(&ret, &hmd_transform);
	ret = api->godot_transform_operator_multiply(&ret, &transform_for_eye);
	
	return ret;
}

void arvr_set_frustum(godot_real *p_projection, godot_real p_left, godot_real p_right, godot_real p_bottom, godot_real p_top, godot_real p_near, godot_real p_far) {

	godot_real x = 2 * p_near / (p_right - p_left);
	godot_real y = 2 * p_near / (p_top - p_bottom);

	godot_real a = (p_right + p_left) / (p_right - p_left);
	godot_real b = (p_top + p_bottom) / (p_top - p_bottom);
	godot_real c = -(p_far + p_near) / (p_far - p_near);
	godot_real d = -2 * p_far * p_near / (p_far - p_near);

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
    
    // multiply matrices
    // get current projection matrix
    #if 1
    api->godot_vector3_new(&r1, p_projection[0], p_projection[1], p_projection[2]);
    api->godot_vector3_new(&r2, p_projection[4], p_projection[5], p_projection[6]);
    api->godot_vector3_new(&r3, p_projection[8], p_projection[9], p_projection[10]);
    // offset
    //api->godot_vector3_new(&off, p_projection[3],p_projection[7],p_projection[11]);
    api->godot_vector3_new(&off, p_projection[12],p_projection[13],p_projection[14]);
    #else
    api->godot_vector3_new(&r1, p_projection[0], p_projection[4], p_projection[8]);
    api->godot_vector3_new(&r2, p_projection[1], p_projection[5], p_projection[9]);
    api->godot_vector3_new(&r3, p_projection[2], p_projection[6], p_projection[10]);
    // offset
    api->godot_vector3_new(&off, p_projection[3],p_projection[7],p_projection[11]);
    #endif

    api->godot_basis_set_row(&basis, 0, &r1);
    api->godot_basis_set_row(&basis, 1, &r2);
    api->godot_basis_set_row(&basis, 2, &r3);

    
    godot_transform projection;
    api->godot_transform_new(&projection, &basis, &off);

    
    godot_transform M;
    api->godot_transform_new(&M, &basis, &off);

    auto matString = api->godot_transform_as_string(&M);
    //printf("%s\n", api->godot_string_ascii(&matString));
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
    p_projection[3] = 0; //api->godot_vector3_get_axis(&off, godot_vector3_axis::GODOT_VECTOR3_AXIS_X);

    p_projection[4] = api->godot_vector3_get_axis(&r2, godot_vector3_axis::GODOT_VECTOR3_AXIS_X);
    p_projection[5] = api->godot_vector3_get_axis(&r2, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y);
    p_projection[6] = api->godot_vector3_get_axis(&r2, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z);
    p_projection[7] = 0; //api->godot_vector3_get_axis(&off, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y);

    p_projection[8] = api->godot_vector3_get_axis(&r3, godot_vector3_axis::GODOT_VECTOR3_AXIS_X);
    p_projection[9] = api->godot_vector3_get_axis(&r3, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y);
    p_projection[10] = api->godot_vector3_get_axis(&r3, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z);
    p_projection[11] = -1; //api->godot_vector3_get_axis(&off, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z);
    
    p_projection[12] = api->godot_vector3_get_axis(&off, godot_vector3_axis::GODOT_VECTOR3_AXIS_X);
    p_projection[13] = api->godot_vector3_get_axis(&off, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y);
    p_projection[14] = api->godot_vector3_get_axis(&off, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z);
    p_projection[15] = 0;

}

void godot_arvr_fill_projection_for_eye(void *p_data, godot_real *p_projection, godot_int p_eye, godot_real p_aspect, godot_real p_z_near, godot_real p_z_far) {
	arvr_data_struct * arvr_data = (arvr_data_struct *) p_data;

    // arbitrary projection from: https://csc.lsu.edu/~kooima/articles/genperspective/
    float l, r, b, t;
    // Distance from the eye to screen plane
    float d;
    //, M[16];

    float n = 0.1; //p_z_near;
    float f = 100; //p_z_far;
    godot_real world_scale = 1;//arvr_api->godot_arvr_get_worldscale();

    //godot::Vector3 pe = arvr_data->pe;
    godot_transform eye_transf;
    api->godot_transform_new_identity(&eye_transf);
    eye_transf = godot_arvr_get_transform_for_eye(p_data, p_eye, &eye_transf);
    auto eye_pos = api->godot_transform_get_origin(&eye_transf);
    godot::Vector3 pe;
    pe.x = api->godot_vector3_get_axis(&eye_pos, godot_vector3_axis::GODOT_VECTOR3_AXIS_X);
    pe.y = api->godot_vector3_get_axis(&eye_pos, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y);
    pe.z = api->godot_vector3_get_axis(&eye_pos, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z);
    printf("eye: %f, %f, %f\n", pe.x, pe.y, pe.z);
    

    // Compute an orthonormal basis for the screen.
    arvr_data->vr = arvr_data->pb - arvr_data->pa;
    arvr_data->vu = arvr_data->pc - arvr_data->pa;
    // printf("p-abc\n");
    // printf("rot: %f, %f, %f\n", arvr_data->pa.x, arvr_data->pa.y, arvr_data->pa.z);
    // printf("rot: %f, %f, %f\n", arvr_data->pb.x, arvr_data->pb.y, arvr_data->pb.z);
    // printf("rot: %f, %f, %f\n", arvr_data->pc.x, arvr_data->pc.y, arvr_data->pc.z);
    // printf("v-ru\n");
    // printf("rot: %f, %f, %f\n", arvr_data->vr.x, arvr_data->vr.y, arvr_data->vr.z);
    // printf("rot: %f, %f, %f\n", arvr_data->vu.x, arvr_data->vu.y, arvr_data->vu.z);

    arvr_data->vr.normalize();
    arvr_data->vu.normalize();

    arvr_data->vn = arvr_data->vr.cross(arvr_data->vu);
    arvr_data->vn.normalize();

    // Compute the screen corner vectors.
    arvr_data->va = arvr_data->pa - pe;
    arvr_data->vb = arvr_data->pb - pe;
    arvr_data->vc = arvr_data->pc - pe;

    // Find the distance from the eye to screen plane.
    d = - arvr_data->vn.dot(arvr_data->va);
    
    // Find the extent of the perpendicular projection.
    l = arvr_data->vr.dot(arvr_data->va) * n / d;
    r = arvr_data->vr.dot(arvr_data->vb) * n / d;
    b = arvr_data->vu.dot(arvr_data->va) * n / d;
    t = arvr_data->vu.dot(arvr_data->vc) * n / d;
    printf("lrbt: %f,%f,%f,%f\n", l,r,b,t);

    // Load the perpendicular projection.
    arvr_set_frustum(p_projection, l, r, b, t, n, f);
    // printf("in\n");
    // printCam(p_projection);


    // camera move
    godot_vector3 move;
    api->godot_vector3_new(&move, -arvr_data->pe.x, -arvr_data->pe.y, -arvr_data->pe.z);
	// P = api->godot_transform_translated(&P, &move);
    godot_transform translation;
    api->godot_transform_new_identity(&translation);
	translation = api->godot_transform_translated(&translation, &move);
    //P = api->godot_transform_operator_multiply(&P, &translation);


    
    // Rotate the projection to be non-perpendicular.
    godot_basis basis;
    api->godot_basis_new(&basis);
    godot_vector3 r1, r2, r3, off;
    // transposed rotation matrix
    api->godot_vector3_new(&r1, arvr_data->vr.x, arvr_data->vr.y, arvr_data->vr.z);
    api->godot_vector3_new(&r2, arvr_data->vu.x, arvr_data->vu.y, arvr_data->vu.z);
    api->godot_vector3_new(&r3, arvr_data->vn.x, arvr_data->vn.y, arvr_data->vn.z);
    // printf("rot:\n");
    // printf("%f, %f, %f\n", arvr_data->vr.x, arvr_data->vr.y, arvr_data->vr.z);
    // printf("%f, %f, %f\n", arvr_data->vu.x, arvr_data->vu.y, arvr_data->vu.z);
    // printf("%f, %f, %f\n", arvr_data->vn.x, arvr_data->vn.y, arvr_data->vn.z);

    // api->godot_vector3_new(&r1, arvr_data->vr.x, arvr_data->vu.x, arvr_data->vn.x);
    // api->godot_vector3_new(&r2, arvr_data->vr.y, arvr_data->vu.y, arvr_data->vn.y);
    // api->godot_vector3_new(&r3, arvr_data->vr.z, arvr_data->vu.z, arvr_data->vn.z);

    api->godot_basis_set_row(&basis, 0, &r1);
    api->godot_basis_set_row(&basis, 1, &r2);
    api->godot_basis_set_row(&basis, 2, &r3);

    // we use the offset here to move the camera into the middle of our frustum
    //api->godot_vector3_new(&off, -arvr_data->pe.x, -arvr_data->pe.y, -arvr_data->pe.z);
    api->godot_vector3_new(&off, 0,0,0);
    godot_transform Mt;
    api->godot_transform_new(&Mt, &basis, &off);

    godot_transform P = camToTransform(p_projection);


    //P = api->godot_transform_operator_multiply(&translation, &P);
    P = api->godot_transform_operator_multiply(&P, &Mt);

    
    
    // back to opengl
    TransformToCam(P, p_projection);
    // printf("out\n");
    // printCam(p_projection);

}

void godot_arvr_commit_for_eye(void *p_data, godot_int p_eye, godot_rid *p_render_target, godot_rect2 *p_screen_rect) {
	// This function is responsible for outputting the final render buffer for each eye. 
	// p_screen_rect will only have a value when we're outputting to the main viewport.

	// For an interface that must output to the main viewport (such as with mobile VR) we should give an error when p_screen_rect is not set
	// For an interface that outputs to an external device we should render a copy of one of the eyes to the main viewport if p_screen_rect is set, and only output to the external device if not.

	arvr_api->godot_arvr_blit(p_eye, p_render_target, p_screen_rect);
}


void godot_arvr_process(void *p_data) {
	// this method gets called before every frame is rendered, here is where you should update tracking data, update controllers, etc.
    //char *p_device_name, godot_int p_hand, godot_bool p_tracks_orientation, godot_bool p_tracks_position);
    // arvr_api->godot_arvr_add_controller("screen_bl", 3, false, true);
    // arvr_api->godot_
    //Godot::Node Scene


    
    godot::SceneTree* tree  = (godot::SceneTree*)godot::Engine::get_singleton()->get_main_loop();
    auto a = tree->get_root();
    
    //godot::String originPath = "Spatial/robot/camera_base/camera_rot/Camera/ARVROrigin";
    godot::String originPath = "Main/ARVROrigin";
    auto camera = ((godot::Spatial*)a->get_node(originPath + "/ARVRCamera"));
    arvr_data_struct * arvr_data = (arvr_data_struct *) p_data;
    arvr_data->pe = camera->get_translation();
    auto r = camera->get_rotation();
    api->godot_vector3_set_axis(&arvr_data->re, godot_vector3_axis::GODOT_VECTOR3_AXIS_X, r.x);
    api->godot_vector3_set_axis(&arvr_data->re, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y, r.y);
    api->godot_vector3_set_axis(&arvr_data->re, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z, r.z);

    auto pa = ((godot::Spatial*)a->get_node(originPath + "/Screen/pa"));
    auto pb = ((godot::Spatial*)a->get_node(originPath + "/Screen/pb"));
    auto pc = ((godot::Spatial*)a->get_node(originPath + "/Screen/pc"));
    arvr_data->pa = pa->get_translation();
    arvr_data->pb = pb->get_translation();
    arvr_data->pc = pc->get_translation();

    // auto screen = a->get_node("Main/ARVROrigin/Screen");
    // printf("%s\n", screen->get_name().alloc_c_string());
    // //godot::Godot::print(((godot::ArvrScreen*)screen)->get_width());
    // printf("%f", ((godot::ArvrScreen*)screen)->get_width());

}

const godot_arvr_interface_gdnative interface_struct = {
	GODOTVR_API_MAJOR, GODOTVR_API_MINOR,
	godot_arvr_constructor,
	godot_arvr_destructor,
	godot_arvr_get_name,
	godot_arvr_get_capabilities,
	godot_arvr_get_anchor_detection_is_enabled,
	godot_arvr_set_anchor_detection_is_enabled,
	godot_arvr_is_stereo,
	godot_arvr_is_initialized,
	godot_arvr_initialize,
	godot_arvr_uninitialize,
	godot_arvr_get_recommended_render_targetsize,
	godot_arvr_get_transform_for_eye,
	godot_arvr_fill_projection_for_eye,
	godot_arvr_commit_for_eye,
	godot_arvr_process
};
