extends Node

var kinect

onready var color_test = $ColorTest
var color_texture = ImageTexture.new()

onready var depth_test = $DepthTest
var depth_texture = ImageTexture.new()

onready var depth_space_test = $DepthSpaceTest
var depth_space_texture = ImageTexture.new()

onready var body_index_test = $BodyIndexTest
var body_index_texture = ImageTexture.new()

onready var center_line = $BodyTest/CenterLine
onready var arm_line = $BodyTest/ArmLine
onready var leg_line = $BodyTest/LegLine


var timer = 0.0

func _ready():
	kinect = load("res://bin/KinectV2.gdns").new()
	color_test.texture = color_texture
	depth_test.texture = depth_texture
	depth_space_test.texture = depth_space_texture
	body_index_test.texture = body_index_texture
	
	var points = PoolVector2Array()
	points.resize(5)
	center_line.points = points
	
	points.resize(11)
	arm_line.points = points
	
	points.resize(9)
	leg_line.points = points

func _process(delta):
	timer += delta
	if timer > 1:
		print("fps: " + str(Engine.get_frames_per_second()))
		timer = 0.0
	
	kinect.update()
	color_texture.create_from_image(kinect.get_color_image())
	depth_texture.create_from_image(kinect.get_depth_image())
	depth_space_texture.create_from_image(kinect.get_depth_space_index_image())
	body_index_texture.create_from_image(kinect.get_body_index_image())
	
	for i in range(6):
		if !kinect.is_tracked(i):
			continue
		
		center_line.set_point_position(0, kinect.get_joint_depth_position(i, 0))
		center_line.set_point_position(1, kinect.get_joint_depth_position(i, 1))
		center_line.set_point_position(2, kinect.get_joint_depth_position(i, 20))
		center_line.set_point_position(3, kinect.get_joint_depth_position(i, 2))
		center_line.set_point_position(4, kinect.get_joint_depth_position(i, 3))
		
		arm_line.set_point_position(0, kinect.get_joint_depth_position(i, 21))
		arm_line.set_point_position(1, kinect.get_joint_depth_position(i, 7))
		arm_line.set_point_position(2, kinect.get_joint_depth_position(i, 6))
		arm_line.set_point_position(3, kinect.get_joint_depth_position(i, 5))
		arm_line.set_point_position(4, kinect.get_joint_depth_position(i, 4))
		arm_line.set_point_position(5, kinect.get_joint_depth_position(i, 20))
		arm_line.set_point_position(6, kinect.get_joint_depth_position(i, 8))
		arm_line.set_point_position(7, kinect.get_joint_depth_position(i, 9))
		arm_line.set_point_position(8, kinect.get_joint_depth_position(i, 10))
		arm_line.set_point_position(9, kinect.get_joint_depth_position(i, 11))
		arm_line.set_point_position(10, kinect.get_joint_depth_position(i, 23))
		
		leg_line.set_point_position(0, kinect.get_joint_depth_position(i, 15))
		leg_line.set_point_position(1, kinect.get_joint_depth_position(i, 14))
		leg_line.set_point_position(2, kinect.get_joint_depth_position(i, 13))
		leg_line.set_point_position(3, kinect.get_joint_depth_position(i, 12))
		leg_line.set_point_position(4, kinect.get_joint_depth_position(i, 0))
		leg_line.set_point_position(5, kinect.get_joint_depth_position(i, 16))
		leg_line.set_point_position(6, kinect.get_joint_depth_position(i, 17))
		leg_line.set_point_position(7, kinect.get_joint_depth_position(i, 18))
		leg_line.set_point_position(8, kinect.get_joint_depth_position(i, 19))
