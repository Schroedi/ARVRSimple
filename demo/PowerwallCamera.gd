extends ARVRCamera

const vrpnClient = preload("res://bin/vrpnClient.gdns")

var inst:vrpnClient = null
func _ready():
	print(vrpnClient)
	#inst = vrpnClient.new()
	#inst.connect("Tracker0@127.0.0.1")
	#inst.connect("UserA@134.102.222.87")
	#OS.window_position = Vector2(1920, 0)
	OS.window_position = Vector2(0, 0)
	OS.window_size = Vector2(2560*2	, 1600)
	#OS.execute("/home/ascadian/Installs/bin/powerMove.sh", ["Powerwall"], false)

func _process(delta):
	#inst.mainloop()
	
#	#print(inst.analog)
#	#print(inst.pos)
#	# depending on trackin system coordinates
#	translation.x = -inst.pos[0]
#	translation.y = inst.pos[1]
#	translation.z = -inst.pos[2]
#	print(inst.pos)
#	#print(inst.button)
#
#	#print(inst.quat)
#	var rot = Quat(inst.quat[0], inst.quat[1], inst.quat[2], inst.quat[3]).get_euler()
#	# depending on trackin system coordinates
#	rotation = Vector3(rot.x, rot.y, -rot.z)
	pass
