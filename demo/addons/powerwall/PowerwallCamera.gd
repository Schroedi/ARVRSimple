extends ARVRCamera
class_name PowerwallCamera

var powerwall = preload("res://addons/powerwall/powerwall.gdns").new()

export var debug_disable_window_position = false
export var debug_emulate_vrpn = false

func _ready():	
	# Find the Powerwall interface
	var arvr_interface = ARVRServer.find_interface("Powerwall")
	if arvr_interface and arvr_interface.initialize():
		get_viewport().arvr = true
		if not debug_emulate_vrpn:
			powerwall.set_tracker_url("UserB@tcp:134.102.222.124")
	else:
		print("Could not start powerwall interface")
	
	if debug_disable_window_position:
		OS.window_borderless = false
		OS.window_position = Vector2(0, 0)
		#OS.window_size = Vector2(1920, 1080)
		OS.window_size = Vector2(5120, 2160)
		pass
	else:
		OS.window_borderless = true
		OS.window_position = Vector2(1920, 0)
		OS.window_size = Vector2(2560*2, 1600)
		powerwall.set_swap_eyes(true)

func _process(delta):
	if (Input.is_action_just_pressed("edge_mode")):
		var curr = powerwall.get_edge_adjust()
		curr = curr ^ 0x1
		powerwall.set_edge_adjust(curr)
		print("edge_adjust %s" % str((curr & 0x1)>0))
	if (Input.is_action_just_pressed("edge_debug")):
			var curr = powerwall.get_edge_adjust()
			curr = curr ^ 0x2
			powerwall.set_edge_adjust(curr)
			print("color_debug %s" % str((curr & 0x2)>0))
	if (Input.is_action_just_pressed("w_debug")):
			var curr = powerwall.get_edge_adjust()
			curr = curr ^ 0x4
			powerwall.set_edge_adjust(curr)
			print("divw %s" % str((curr & 0x4)>0))
	if (Input.is_action_just_pressed("trans_pro")):
			var curr = powerwall.get_edge_adjust()
			curr = curr ^ 0x8
			powerwall.set_edge_adjust(curr)
			print("trans_pro %s" % str((curr & 0x8)>0))
		
	# debug movement
#	if (Input.is_key_pressed(KEY_LEFT)):
#		get_parent().rotation.y += delta
#	elif (Input.is_key_pressed(KEY_RIGHT)):
#		get_parent().rotation.y -= delta
#
#	if (Input.is_key_pressed(KEY_R)):
#		get_parent().rotation.x += delta
#	elif (Input.is_key_pressed(KEY_F)):
#		get_parent().rotation.x -= delta

	var speed = 1.0 if Input.is_key_pressed(KEY_SHIFT) else .5
	
	# height
	if (Input.is_key_pressed(KEY_UP) or Input.is_key_pressed(KEY_W)):
		debug_move(Vector3.UP * speed * delta);
	elif (Input.is_key_pressed(KEY_DOWN) or  Input.is_key_pressed(KEY_S)):
		debug_move(Vector3.DOWN * speed * delta);
	
	# strave
	if (Input.is_key_pressed(KEY_A)):
		debug_move(Vector3.LEFT * speed * delta);
	elif (Input.is_key_pressed(KEY_D)):
		debug_move(Vector3.RIGHT * speed * delta);
	
	# front/back
	if (Input.is_key_pressed(KEY_PAGEUP)):
		debug_move(Vector3.FORWARD * speed * delta);
	elif (Input.is_key_pressed(KEY_PAGEDOWN)):
		debug_move(Vector3.BACK * speed * delta);

func debug_move(v:Vector3):
	var transform:Transform = powerwall.get_head_transform()
	powerwall.set_head_transform(transform.translated(v))
