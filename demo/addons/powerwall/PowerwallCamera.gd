extends ARVRCamera

var powerwall = preload("res://addons/powerwall/powerwall.gdns").new()

func _ready():

	# Find the interface
	var arvr_interface = ARVRServer.find_interface("Powerwall")
	if arvr_interface and arvr_interface.initialize():
		get_viewport().arvr = true
		powerwall.set_tracker_url("UserB@tcp:134.102.222.124")
	else:
		print("Could not start powerwall interface")
	
	OS.window_position = Vector2(2560, 0)
	#OS.window_position = Vector2(0, 0)
	OS.window_size = Vector2(2560*2	, 1600)
	#OS.window_size = Vector2(1500	, 1000)
	pass

func _process(delta):
	var head_trans = powerwall.get_head_transform(0)
	if (Input.is_action_just_pressed("edge_mode")):
		var curr = powerwall.get_edge_adjust()
		curr = curr ^ 0x1
		print(powerwall.set_edge_adjust(curr))
	if (Input.is_action_just_pressed("edge_debug")):
			var curr = powerwall.get_edge_adjust()
			curr = curr ^ 0x2
			print(powerwall.set_edge_adjust(curr))
	if (Input.is_action_just_pressed("w_debug")):
			var curr = powerwall.get_edge_adjust()
			curr = curr ^ 0x4
			print(powerwall.set_edge_adjust(curr))
		
	# debug movement
	if (Input.is_key_pressed(KEY_LEFT)):
		get_parent().rotation.y += delta
	elif (Input.is_key_pressed(KEY_RIGHT)):
		get_parent().rotation.y -= delta

	if (Input.is_key_pressed(KEY_R)):
		get_parent().rotation.x += delta
	elif (Input.is_key_pressed(KEY_F)):
		get_parent().rotation.x -= delta


	if (Input.is_key_pressed(KEY_UP) or Input.is_key_pressed(KEY_W)):
		get_parent().translation -= get_parent().transform.basis.z * delta;
	elif (Input.is_key_pressed(KEY_DOWN) or  Input.is_key_pressed(KEY_S)):
		get_parent().translation += get_parent().transform.basis.z * delta;
	
	# strave
	if (Input.is_key_pressed(KEY_A)):
		get_parent().translation -= get_parent().transform.basis.x * delta;
	elif (Input.is_key_pressed(KEY_D)):
		get_parent().translation += get_parent().transform.basis.x * delta;

	if (Input.is_key_pressed(KEY_Q)):
		get_parent().translation -= get_parent().transform.basis.y * delta;
	elif (Input.is_key_pressed(KEY_E)):
		get_parent().translation += get_parent().transform.basis.y * delta;

