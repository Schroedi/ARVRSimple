extends ARVRCamera

const vrpnClient = preload("res://bin/vrpnClient.gdns")

var inst:vrpnClient = null
func _ready():
	print(vrpnClient)
	inst = vrpnClient.new()
	inst.connect("Tracker0@127.0.0.1:3883")
	#inst.connect("UserB@134.102.222.87:3883")

func _process(delta):
	inst.mainloop()
	#print(inst.analog)
	#print(inst.pos)
	translation.x = -inst.pos[0]
	#print(inst.pos[0])
	translation.y = inst.pos[1]+1
	translation.z = -inst.pos[2]
	#print(inst.pos)
	#print(inst.button)