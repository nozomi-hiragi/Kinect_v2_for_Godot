extends Sprite

onready var center_line = $CenterLine
onready var arm_line = $ArmLine
onready var leg_line = $LegLine
var body_source

func _ready():
	body_source = load("res://bin/BodySource.gdns").new()
	
	var points = PoolVector2Array()
	points.resize(5)
	center_line.points = points
	
	points.resize(11)
	arm_line.points = points
	
	points.resize(9)
	leg_line.points = points

func _process(delta):
	if !body_source.update():
		return
	
	for i in range(6):
		if !body_source.is_tracked(i):
			continue
		
		center_line.set_point_position(0, body_source.get_joint_depth_position(i, 0))
		center_line.set_point_position(1, body_source.get_joint_depth_position(i, 1))
		center_line.set_point_position(2, body_source.get_joint_depth_position(i, 20))
		center_line.set_point_position(3, body_source.get_joint_depth_position(i, 2))
		center_line.set_point_position(4, body_source.get_joint_depth_position(i, 3))
		
		arm_line.set_point_position(0, body_source.get_joint_depth_position(i, 21))
		arm_line.set_point_position(1, body_source.get_joint_depth_position(i, 7))
		arm_line.set_point_position(2, body_source.get_joint_depth_position(i, 6))
		arm_line.set_point_position(3, body_source.get_joint_depth_position(i, 5))
		arm_line.set_point_position(4, body_source.get_joint_depth_position(i, 4))
		arm_line.set_point_position(5, body_source.get_joint_depth_position(i, 20))
		arm_line.set_point_position(6, body_source.get_joint_depth_position(i, 8))
		arm_line.set_point_position(7, body_source.get_joint_depth_position(i, 9))
		arm_line.set_point_position(8, body_source.get_joint_depth_position(i, 10))
		arm_line.set_point_position(9, body_source.get_joint_depth_position(i, 11))
		arm_line.set_point_position(10, body_source.get_joint_depth_position(i, 23))
		
		leg_line.set_point_position(0, body_source.get_joint_depth_position(i, 15))
		leg_line.set_point_position(1, body_source.get_joint_depth_position(i, 14))
		leg_line.set_point_position(2, body_source.get_joint_depth_position(i, 13))
		leg_line.set_point_position(3, body_source.get_joint_depth_position(i, 12))
		leg_line.set_point_position(4, body_source.get_joint_depth_position(i, 0))
		leg_line.set_point_position(5, body_source.get_joint_depth_position(i, 16))
		leg_line.set_point_position(6, body_source.get_joint_depth_position(i, 17))
		leg_line.set_point_position(7, body_source.get_joint_depth_position(i, 18))
		leg_line.set_point_position(8, body_source.get_joint_depth_position(i, 19))
