////////////////////////////////////////////////////////////////////////////
// This is just an example ARVR GDNative module. It does very little VR-ish
// It also outputs lots of debugging stuff which should not be in a proper
// module :)

// Written by Bastiaan "Mux213" Olij, with loads of help from Thomas "Karroffel" Herzog

#ifndef ARVR_INTERFACE_H
#define ARVR_INTERFACE_H

#include "GodotCalls.h"
#include <Godot.hpp>

extern const godot_arvr_interface_gdnative interface_struct;

typedef struct arvr_data_struct {
	godot_object * instance;
	bool is_initialised;
	float iod_cm;
	float oversample;


	godot::Vector3 va, vb, vc;
	godot::Vector3 vr, vu, vn;
    // projection screen coordinates
	godot::Vector3 pa, pb, pc;
    // eye coordinates
	godot::Vector3 pe;
    // near and far clip
    float n, f;
    
} arvr_data_struct;

#endif /* !ARVR_INTERFACE_H */
