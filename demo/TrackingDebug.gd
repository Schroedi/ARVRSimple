extends Control

export var PowerwallCamPath: NodePath

var PowerwallCam: PowerwallCamera

onready var lbl_pos = $Position

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	if PowerwallCamPath.is_empty():
		printerr("Can't show tracking data, powerwall cam node path not set")
		lbl_pos.text = "Tracking node setup error"
		set_process(false)
		return
	
	PowerwallCam = get_node(PowerwallCamPath)

func _process(delta: float) -> void:
	var trans:Transform = PowerwallCam.powerwall.get_head_transform()
	lbl_pos.text = str(trans.origin)
	pass
