#pragma once

#include <Godot.hpp>
#include <Node.hpp>

namespace godot {
    
class PowerwallScreen : public Node{
	GODOT_CLASS(PowerwallScreen, Node)
	
public:
	PowerwallScreen();
	~PowerwallScreen(){};

	static void _register_methods();
    void _init();

    void set_width(float width);
    float get_width();
    void set_height(float height);
    float get_height();
    void _enter_tree();
    
private:
    float width;
    float height;
};

}
