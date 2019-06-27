////////////////////////////////////////////////////////////////////////////
// This is just an example POWERWALL GDNative module. It does very little VR-ish
// It also outputs lots of debugging stuff which should not be in a proper
// module :)

// Written by Bastiaan "Mux213" Olij, with loads of help from Thomas "Karroffel" Herzog

#include <cassert>
#include "Powerwall.h"
//#include "PowerwallScreen.h"

typedef struct user_data_struct {
    godot_vector3 pa;
} user_data_struct;

void GDN_EXPORT *simple_constructor(godot_object *p_instance, void *p_method_data) {
    user_data_struct *user_data = static_cast<user_data_struct *>(api->godot_alloc(sizeof(user_data_struct)));
    api->godot_vector3_new(&user_data->pa, 0, 0, 0);
    return user_data;
}

void GDN_EXPORT simple_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data) {
    api->godot_free(p_user_data);
}

// instance pointer, method data, user data, num args, args - return result as varaint
godot_variant GDN_EXPORT mytestfunction(godot_object *p_instance, void *p_method_data,
                                        void *p_user_data, int p_num_args, godot_variant **p_args) {
    user_data_struct * user_data = (user_data_struct *) p_user_data;

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

    godot_instance_create_func create = { NULL, NULL, NULL };
    create.create_func = &simple_constructor;

    godot_instance_destroy_func destroy = { NULL, NULL, NULL };
    destroy.destroy_func = &simple_destructor;

    nativescript_api->godot_nativescript_register_class(p_handle, "Powerwall", "Spatial",
                                                        create, destroy);

    godot_instance_method get_data = { NULL, NULL, NULL };
    get_data.method = &mytestfunction;

    godot_method_attributes attributes = { GODOT_METHOD_RPC_MODE_DISABLED };

    nativescript_api->godot_nativescript_register_method(p_handle, "Powerwall", "set_pa",
                                                         attributes, get_data);
}
