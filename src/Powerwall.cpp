////////////////////////////////////////////////////////////////////////////
// This is just an example POWERWALL GDNative module. It does very little VR-ish
// It also outputs lots of debugging stuff which should not be in a proper
// module :)

// Written by Bastiaan "Mux213" Olij, with loads of help from Thomas "Karroffel" Herzog

#include <cassert>
#include <iostream>
#include "Powerwall.h"

void GDN_EXPORT *simple_constructor(godot_object *p_instance, void *p_method_data) {
    return arvr_data;
}

void GDN_EXPORT simple_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data) {

}

// instance pointer, method data, user data, num args, args - return result as varaint
godot_variant GDN_EXPORT mytestfunction(godot_object *p_instance, void *p_method_data,
                                        void *p_user_data, int p_num_args, godot_variant **p_args) {
    arvr_data_struct * user_data = (arvr_data_struct *) p_user_data;

    assert(p_num_args == 1);
    user_data->pa = api->godot_variant_as_vector3(p_args[0]);

    printf("Native got vec x: %f", api->godot_vector3_get_axis(&user_data->pa, godot_vector3_axis::GODOT_VECTOR3_AXIS_Z));

    godot_variant ret;
    api->godot_variant_new_nil(&ret);
    return ret;
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
    {
        godot_instance_method get_data = {NULL, NULL, NULL};
        get_data.method = &mytestfunction;
        godot_method_attributes attributes = {GODOT_METHOD_RPC_MODE_DISABLED};
        nativescript_api->godot_nativescript_register_method(p_handle, "Powerwall", "set_pa",
                                                             attributes, get_data);
    }
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
}


GDCALLINGCONV godot_variant powerwall_config_get_edge_adjust(godot_object *p_instance, void *p_method_data,
    void *p_user_data, int p_num_args, godot_variant **p_args) {
    godot_variant ret;

    if (p_user_data == NULL) {
        // this should never ever ever ever happen, just being paranoid....
        api->godot_variant_new_bool(&ret, false);
    } else {
        auto *arvr_data = (arvr_data_struct *)p_user_data;
        api->godot_variant_new_bool(&ret, arvr_data->enable_edge_adjust);
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
        bool new_value = api->godot_variant_as_bool(p_args[0]);
        auto *arvr_data = (arvr_data_struct *)p_user_data;
        arvr_data->enable_edge_adjust = new_value;
        std::cout << "Setting edge adjust to " << new_value << std::endl;
        api->godot_variant_new_bool(&ret, true);
    }

    return ret;
}