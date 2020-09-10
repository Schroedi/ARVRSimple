extends Spatial

#var powerwall = preload("res://addons/powerwall/powerwall.gdns").new()

func _ready():
#	# in ARVROrigin.Camera now
#	# Find the interface
#	var arvr_interface = ARVRServer.find_interface("Powerwall")
#	if arvr_interface and arvr_interface.initialize():
#		get_viewport().arvr = true
#	else:
#		print("Could not start powerwall interface")
	pass
	

func _process(delta):
	# Test for escape to close application, space to reset our reference frame
	if (Input.is_key_pressed(KEY_ESCAPE)):
		get_tree().quit()
#	elif (Input.is_key_pressed(KEY_SPACE)):
#		# Calling center_on_hmd will cause the ARVRServer to adjust all tracking data so the player is centered on the origin point looking forward
#		ARVRServer.center_on_hmd(true, true)
#	elif (Input.is_action_just_pressed("edge_mode")):
#		$MeshInstance4.visible = powerwall.get_edge_adjust()
