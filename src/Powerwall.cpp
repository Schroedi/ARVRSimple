////////////////////////////////////////////////////////////////////////////
// This is just an example POWERWALL GDNative module. It does very little VR-ish
// It also outputs lots of debugging stuff which should not be in a proper
// module :)

// Written by Bastiaan "Mux213" Olij, with loads of help from Thomas "Karroffel" Herzog

#include "Powerwall.h"
//#include "PowerwallScreen.h"

void GDN_EXPORT godot_powerwall_gdnative_singleton() {
    if (arvr_api != NULL) {
        arvr_api->godot_arvr_register_interface(&interface_struct);
    }
}

void GDN_EXPORT godot_powerwall_nativescript_init(void *p_handle) {
}
