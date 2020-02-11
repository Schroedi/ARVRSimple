////////////////////////////////////////////////////////////////////////////
// This is just an example ARVR GDNative module. It does very little VR-ish
// It also outputs lots of debugging stuff which should not be in a proper
// module :)

// Written by Bastiaan "Mux213" Olij, with loads of help from Thomas "Karroffel" Herzog

#ifndef POWERWALL_INTERFACE_H
#define POWERWALL_INTERFACE_H

#include <vrpn_Tracker.h>
#include "GodotCalls.h"


typedef struct arvr_data_struct {
    godot_object * instance;
    bool is_initialised;
    float iod_m;

    vrpn_Tracker_Remote *vrpnTracker;

    godot_vector3 va, vb, vc;
    godot_vector3 vr, vu, vn;
    // projection screen coordinates
    godot_vector3 pa, pb, pc;
    // eye coordinates
    godot_vector3 pe;
    // eye rotation (X-angle, Y-angle, Z-angle)
    godot_quat re;
    // near and far clip
    float n, f;
    godot_int enable_edge_adjust;
    godot_string tracker_url;
} arvr_data_struct;

extern arvr_data_struct *arvr_data;

void *godot_arvr_constructor(godot_object *p_instance);

void godot_arvr_destructor(void *p_data);

godot_string godot_arvr_get_name(const void *p_data);

godot_int godot_arvr_get_capabilities(const void *p_data);

godot_bool godot_arvr_get_anchor_detection_is_enabled(const void *p_data);

void godot_arvr_set_anchor_detection_is_enabled(void *p_data, bool p_enable);

godot_bool godot_arvr_is_stereo(const void *p_data);

godot_bool godot_arvr_is_initialized(const void *p_data);

godot_bool godot_arvr_initialize(void *p_data);

void godot_arvr_uninitialize(void *p_data);

godot_vector2 godot_arvr_get_render_targetsize(const void *p_data);

godot_transform godot_arvr_get_transform_for_eye(void *p_data, godot_int p_eye, godot_transform *p_cam_transform);

void godot_arvr_fill_projection_for_eye(void *p_data, godot_real *p_projection, godot_int p_eye, godot_real p_aspect,
                                        godot_real p_z_near, godot_real p_z_far);

godot_int godot_arvr_get_external_texture_for_eye(void *p_data, godot_int p_eye);

void godot_arvr_commit_for_eye(void *p_data, godot_int p_eye, godot_rid *p_render_target, godot_rect2 *p_screen_rect);

void godot_arvr_process(void *p_data);

void godot_arvr_notification(void *p_data, godot_int p_what);

const godot_arvr_interface_gdnative interface_struct = {
    { GODOTVR_API_MAJOR,
      GODOTVR_API_MINOR },
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
    godot_arvr_get_render_targetsize,
    godot_arvr_get_transform_for_eye,
    godot_arvr_fill_projection_for_eye,
    godot_arvr_commit_for_eye,
    godot_arvr_process,
    godot_arvr_get_external_texture_for_eye,
    godot_arvr_notification
};

#endif /* !POWERWALL_INTERFACE_H */
