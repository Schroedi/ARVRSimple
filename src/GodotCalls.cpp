////////////////////////////////////////////////////////////////////////////////////////////////
// Just some code setting up access to our APIs in Godot

#include <Godot.hpp>
#include "GodotCalls.h"

//const godot_gdnative_core_api_struct *api = NULL;
const godot_gdnative_ext_arvr_api_struct *arvr_api = NULL;
//const godot_gdnative_ext_nativescript_api_struct *nativescript_api = NULL;

extern "C" void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *p_options) {
    godot::Godot::gdnative_init(p_options);
	// get our main API struct
	//api = p_options->api_struct;

	// now find our arvr extension
	for (int i = 0; i < api->num_extensions; i++) {
		// todo: add version checks
		switch (api->extensions[i]->type) {
			case GDNATIVE_EXT_ARVR: {
				if (api->extensions[i]->version.major > 1 || (api->extensions[i]->version.major == 1 && api->extensions[i]->version.minor >= 1)) {
					arvr_api = (godot_gdnative_ext_arvr_api_struct *)api->extensions[i];
				} else {
					printf("ARVR API version %i.%i isn't supported, need version 1.1 or higher\n", api->extensions[i]->version.major, api->extensions[i]->version.minor);
				}
			}; break;
			case GDNATIVE_EXT_NATIVESCRIPT: {
				if (api->extensions[i]->version.major > 1 || (api->extensions[i]->version.major == 1 && api->extensions[i]->version.minor >= 0)) {
					//nativescript_api = (godot_gdnative_ext_nativescript_api_struct *)api->extensions[i];
				} else {
					printf("Native script API version %i.%i isn't supported, need version 1.1 or higher\n", api->extensions[i]->version.major, api->extensions[i]->version.minor);
				}
			}; break;
			default: break;
		}
	}
}

extern "C" void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *p_options) {
    godot::Godot::gdnative_terminate(p_options);
    //api = NULL;
	//nativescript_api = NULL;
	arvr_api = NULL;
}


