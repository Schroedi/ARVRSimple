#include "ARVRScreen.h"
#include <core/String.hpp>

using namespace godot;

ArvrScreen::ArvrScreen() {
    printf("Construct screen");
}

void ArvrScreen::_register_methods() {
    //    register_method("init", M method_ptr)
    //register_method("_process", &ArvrScreen::_process);
    register_property<ArvrScreen, float>("screenWidth", &ArvrScreen::set_width, &ArvrScreen::get_width, 4.0);
    register_property<ArvrScreen, float>("screenHeight", &ArvrScreen::set_height, &ArvrScreen::get_height, 3.0);
}

void ArvrScreen::_enter_tree() {
    
}

void ArvrScreen::_init() {
    // initialize any variables here
    Godot::print("init");
    width = 442;
}

void ArvrScreen::set_width(float p_width) {
    this->width = p_width;
    Godot::print("setw: " + String::num(this->width));
    printf("%p", this);
}
float ArvrScreen::get_width() {
    
    //Godot::print("getw: " + String::num(width));
    //printf("%p", this);
    return this->width;
}
void ArvrScreen::set_height(float p_height) {
    this->height = p_height;
}
float ArvrScreen::get_height() {
    return this->height;
}
