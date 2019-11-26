extends Spatial

const vrpnClient = preload("res://bin/vrpnClient.gdns")

var inst:vrpnClient = null
func _ready():
	print(vrpnClient)
	inst = vrpnClient.new()
	inst.connect("Tracker0@127.0.0.1:3883")

func _process(delta):
	inst.mainloop()
	#print(inst.analog)
	#print(inst.pos)
	get_parent().translation.x = inst.pos[0]
	get_parent().translation.y = inst.pos[2]
	get_parent().translation.z = -inst.pos[1]
	#print(inst.pos)
	#print(inst.button)
