////////////////////////////////////////////////////////////////////////////
// This is just an example POWERWALL GDNative module. It does very little VR-ish
// It also outputs lots of debugging stuff which should not be in a proper
// module :)

// Written by Bastiaan "Mux213" Olij, with loads of help from Thomas "Karroffel" Herzog

#include <cassert>
#include <iostream>
#include "Powerwall.h"

void GDN_EXPORT *simple_constructor(godot_object *p_instance, void *p_method_data) {
    return g_arvr_data;
}

void GDN_EXPORT simple_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data) {

}

void GDN_EXPORT godot_powerwall_gdnative_singleton() {
    if (arvr_api != NULL) {
        arvr_api->godot_arvr_register_interface(&interface_struct);
    }
}

void GDN_EXPORT godot_powerwall_nativescript_init(void *p_handle) {
    // Register class
    godot_instance_create_func create = { NULL, NULL, NULL };
    create.create_func = &simple_constructor;
    godot_instance_destroy_func destroy = { NULL, NULL, NULL };
    destroy.destroy_func = &simple_destructor;
    nativescript_api->godot_nativescript_register_class(p_handle, "Powerwall", "Spatial",
                                                        create, destroy);

    // Methods
    // edge mode
    {
        godot_instance_method get_data = {NULL, NULL, NULL};
        get_data.method = &powerwall_config_set_edge_adjust;
        godot_method_attributes attributes = {GODOT_METHOD_RPC_MODE_DISABLED};
        nativescript_api->godot_nativescript_register_method(p_handle, "Powerwall", "set_edge_adjust",
                                                             attributes, get_data);
    }
    {
        godot_instance_method get_data = {NULL, NULL, NULL};
        get_data.method = &powerwall_config_get_edge_adjust;
        godot_method_attributes attributes = {GODOT_METHOD_RPC_MODE_DISABLED};
        nativescript_api->godot_nativescript_register_method(p_handle, "Powerwall", "get_edge_adjust",
                                                             attributes, get_data);
    }

    // head transform
    {
        godot_instance_method get_data = {NULL, NULL, NULL};
        get_data.method = &powerwall_set_head_transform;
        godot_method_attributes attributes = {GODOT_METHOD_RPC_MODE_DISABLED};
        nativescript_api->godot_nativescript_register_method(p_handle, "Powerwall", "set_head_transform",
                                                             attributes, get_data);
    }
    {
        godot_instance_method get_data = {NULL, NULL, NULL};
        get_data.method = &powerwall_get_head_transform;
        godot_method_attributes attributes = {GODOT_METHOD_RPC_MODE_DISABLED};
        nativescript_api->godot_nativescript_register_method(p_handle, "Powerwall", "get_head_transform",
                                                             attributes, get_data);
    }

    // ipd
    {
        godot_instance_method get_data = {NULL, NULL, NULL};
        get_data.method = &powerwall_config_set_ipd;
        godot_method_attributes attributes = {GODOT_METHOD_RPC_MODE_DISABLED};
        nativescript_api->godot_nativescript_register_method(p_handle, "Powerwall", "set_ipd",
                                                             attributes, get_data);
    }

    // tracker
    {
        godot_instance_method get_data = {NULL, NULL, NULL};
        get_data.method = &powerwall_config_set_tracker_url;
        godot_method_attributes attributes = {GODOT_METHOD_RPC_MODE_DISABLED};
        nativescript_api->godot_nativescript_register_method(p_handle, "Powerwall", "set_tracker_url",
                                                             attributes, get_data);
    }

    // swap_eyes
    {
        godot_instance_method get_data = {NULL, NULL, NULL};
        get_data.method = &powerwall_config_set_swap_eyes;
        godot_method_attributes attributes = {GODOT_METHOD_RPC_MODE_DISABLED};
        nativescript_api->godot_nativescript_register_method(p_handle, "Powerwall", "set_swap_eyes",
                                                             attributes, get_data);
    }
}


GDCALLINGCONV godot_variant powerwall_config_get_edge_adjust(godot_object *p_instance, void *p_method_data,
                                                             void *p_user_data, int p_num_args, godot_variant **p_args) {
    godot_variant ret;

    if (p_user_data == NULL) {
        // this should never ever ever ever happen, just being paranoid....
        api->godot_variant_new_int(&ret, 0);
    } else {
        auto *arvr_data = (arvr_data_struct *)p_user_data;
        api->godot_variant_new_int(&ret, arvr_data->enable_edge_adjust);
    }

    return ret;
}

GDCALLINGCONV godot_variant powerwall_config_set_edge_adjust(godot_object *p_instance, void *p_method_data,
                                                             void *p_user_data, int p_num_args, godot_variant **p_args) {
    godot_variant ret;

    if (p_user_data == NULL) {
        // this should never ever ever ever happen, just being paranoid....
        api->godot_variant_new_bool(&ret, false);
    } else if (p_num_args == 0) {
        // no arguments given
        api->godot_variant_new_bool(&ret, false);
    } else {
        int new_value = api->godot_variant_as_int(p_args[0]);
        auto *arvr_data = (arvr_data_struct *)p_user_data;
        arvr_data->enable_edge_adjust = new_value;

        bool edgeAdjust = (new_value & 0x1) > 0;
        bool color_debug = (new_value & 0x2) > 0;
        bool div_w = (new_value & 0x4) > 0;
        bool trans_pro = (new_value & 0x8) > 0;
        std::cout << "Setting edge adjust to " << std::endl;
        std::cout << "edgeAdjust = " << edgeAdjust << std::endl;
        std::cout << "color_debug = " << color_debug << std::endl;
        std::cout << "div_w = " << div_w << std::endl;
        std::cout << "trans_pro = " << trans_pro << std::endl;

        api->godot_variant_new_bool(&ret, true);
    }

    return ret;
}

//GDCALLINGCONV godot_variant powerwall_config_get_tracker_url(godot_object *p_instance, void *p_method_data,
//                                                             void *p_user_data, int p_num_args, godot_variant **p_args) {
//    godot_variant ret;
//
//    if (p_user_data == NULL) {
//        // this should never ever ever ever happen, just being paranoid....
//        api->godot_variant_new_bool(&ret, false);
//    } else {
//        auto *g_arvr_data = (arvr_data_struct *)p_user_data;
//        api->godot_variant_new_string(&ret, &g_arvr_data->tracker_url);
//    }
//
//    return ret;
//}

void VRPN_CALLBACK tracker_callback(void* p_data, const vrpn_TRACKERCB t ){
    auto *arvr_data = (arvr_data_struct *)p_data;

    api->godot_vector3_set_axis(&arvr_data->pe, godot_vector3_axis::GODOT_VECTOR3_AXIS_X, t.pos[0]);
    api->godot_vector3_set_axis(&arvr_data->pe, godot_vector3_axis::GODOT_VECTOR3_AXIS_Y, t.pos[2]);
    api->godot_vector3_set_axis(&arvr_data->pe, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z, -t.pos[1]);

    api->godot_quat_set_x(&arvr_data->re, t.quat[0]);
    api->godot_quat_set_y(&arvr_data->re, t.quat[2]);
    api->godot_quat_set_z(&arvr_data->re, t.quat[1]);
    api->godot_quat_set_w(&arvr_data->re, t.quat[3]);
}

GDCALLINGCONV godot_variant powerwall_config_set_tracker_url(godot_object *p_instance, void *p_method_data,
                                                             void *p_user_data, int p_num_args, godot_variant **p_args) {
    godot_variant ret;

    if (p_user_data == NULL) {
        // this should never ever ever ever happen, just being paranoid....
        api->godot_variant_new_bool(&ret, false);
    } else if (p_num_args == 0) {
        // no arguments given
        api->godot_variant_new_bool(&ret, false);
    } else {
        godot_string new_value = api->godot_variant_as_string(p_args[0]);
        auto *arvr_data = (arvr_data_struct *)p_user_data;
        arvr_data->tracker_url = new_value;
        api->godot_print(&new_value);

        // init vrpn
        if (arvr_data->vrpnTracker) {
            arvr_data->vrpnTracker->unregister_change_handler(p_user_data, tracker_callback);
            delete arvr_data->vrpnTracker;
            arvr_data->vrpnTracker = nullptr;
        }
        godot_char_string char_string = api->godot_string_utf8(&arvr_data->tracker_url);
        arvr_data->vrpnTracker = new vrpn_Tracker_Remote(api->godot_char_string_get_data(&char_string) );
        if(arvr_data->vrpnTracker == nullptr){
            std::cout << "vrpnServer() Error: trackerVrpnServer could not be created" << std::endl;
        } else {
            arvr_data->vrpnTracker->register_change_handler(p_user_data, tracker_callback);
        }

        api->godot_variant_new_bool(&ret, true);
    }

    return ret;
}

/*GDCALLINGCONV godot_variant powerwall_get_eye_projection(godot_object *p_instance, void *p_method_data,
                                                             void *p_user_data, int p_num_args, godot_variant **p_args) {
    godot_variant ret;

    if (p_user_data == NULL) {
        // this should never ever ever ever happen, just being paranoid....
        api->godot_variant_new_bool(&ret, false);
    } else if (p_num_args == 0) {
        // no arguments given
        api->godot_variant_new_bool(&ret, false);
    } else {
        godot_int eye = api->godot_variant_as_int(p_args[0]);
        auto *g_arvr_data = (arvr_data_struct *)p_user_data;
        godot_real projection[16];
        float aspect = 1; // not used
        float near = 0.1;
        float far = 500;
        godot_arvr_fill_projection_for_eye(g_arvr_data, projection, eye, aspect, near, far);


        //api->godot_variant_new_pool_real_array(&ret, &g_arvr_data->tracker_url);
    }

    return ret;
}*/


GDCALLINGCONV godot_variant powerwall_get_head_transform(godot_object *p_instance, void *p_method_data,
                                                         void *p_user_data, int p_num_args, godot_variant **p_args) {
    godot_variant ret;

    if (p_user_data == NULL) {
        // this should never ever ever ever happen, just being paranoid....
        api->godot_variant_new_bool(&ret, false);
    } else if (p_num_args != 0) {
        // no arguments given
        api->godot_variant_new_bool(&ret, false);
    } else {
        auto *arvr_data = (arvr_data_struct *) p_user_data;

        godot_basis head_rotation;
        api->godot_basis_new_with_euler_quat(&head_rotation, &arvr_data->re);

        godot_transform head_transform;
        api->godot_transform_new(&head_transform, &head_rotation, &arvr_data->pe);

        api->godot_variant_new_transform(&ret, &head_transform);
    }

    return ret;
}

GDCALLINGCONV godot_variant powerwall_set_head_transform(godot_object *p_instance, void *p_method_data,
                                                             void *p_user_data, int p_num_args, godot_variant **p_args) {
    godot_variant ret;

    if (p_user_data == NULL) {
        // this should never ever ever ever happen, just being paranoid....
        api->godot_variant_new_bool(&ret, false);
    } else if (p_num_args != 1) {
        // no arguments given
        api->godot_variant_new_bool(&ret, false);
    } else {
        godot_transform transform = api->godot_variant_as_transform(p_args[0]);
        auto *arvr_data = (arvr_data_struct *)p_user_data;
        // FIXME: overwrite transform, set overwrite to true and save transform
        //g_arvr_data->enable_edge_adjust = new_value;
        godot_basis head_rotation = api->godot_transform_get_basis(&transform);
        arvr_data->pe = api->godot_transform_get_origin(&transform);

        std::cout << "Setting transform " << std::endl;
        api->godot_variant_new_bool(&ret, true);
    }

    return ret;
}

GDCALLINGCONV godot_variant powerwall_config_set_ipd(godot_object *p_instance, void *p_method_data,
                                                             void *p_user_data, int p_num_args, godot_variant **p_args) {
    godot_variant ret;

    if (p_user_data == NULL) {
        // this should never ever ever ever happen, just being paranoid....
        api->godot_variant_new_bool(&ret, false);
    } else if (p_num_args == 0) {
        // no arguments given
        api->godot_variant_new_bool(&ret, false);
    } else {
        godot_real new_value = api->godot_variant_as_real(p_args[0]);
        auto *arvr_data = (arvr_data_struct *)p_user_data;
        arvr_data->iod_m = new_value;
        std::cout << "Setting edge ipd to " << new_value << std::endl;
        api->godot_variant_new_bool(&ret, true);
    }

    return ret;
}

GDCALLINGCONV godot_variant powerwall_config_set_swap_eyes(godot_object *p_instance, void *p_method_data,
                                                             void *p_user_data, int p_num_args, godot_variant **p_args) {
    godot_variant ret;

    if (p_user_data == NULL) {
        // this should never ever ever ever happen, just being paranoid....
        api->godot_variant_new_bool(&ret, false);
    } else if (p_num_args == 0) {
        // no arguments given
        api->godot_variant_new_bool(&ret, false);
    } else {
        bool new_value = api->godot_variant_as_bool(p_args[0]);
        auto *arvr_data = (arvr_data_struct *)p_user_data;
        arvr_data->swap_eyes = new_value;
        api->godot_variant_new_bool(&ret, true);
    }

    return ret;
}