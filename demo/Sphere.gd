extends MeshInstance

func _process(delta: float) -> void:
	translation.z = sin(OS.get_ticks_msec()/1000.0) / 2.0
