extends ARVRCamera

var powerwall = preload("res://addons/powerwall/powerwall.gdns").new()

func _ready():

	# Find the interface
	var arvr_interface = ARVRServer.find_interface("Powerwall")
	if arvr_interface and arvr_interface.initialize():
		get_viewport().arvr = true
	else:
		print("Could not start powerwall interface")
	
	#OS.window_position = Vector2(1920, 0)
	#OS.window_position = Vector2(0, 0)
	#OS.window_size = Vector2(2560*2	, 1600)
	#OS.window_size = Vector2(1500	, 1000)

func _process(delta):
	print("proc")
	pass
