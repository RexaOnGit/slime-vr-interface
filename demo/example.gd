extends Node

var avatar_path = "C:/Users/kyler/source/repos/slime-vr-interface/demo/assets/avatar_glbs/avatar_sample_b/AvatarSample_B.glb"

var Interface := SlimeVRInterface.new()

func _ready() -> void:

	Interface.setAvatarResourcePath(avatar_path)

	Interface.start()
	
	#Interface.avatarInit(self)
	
