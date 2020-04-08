extends Node

var kinect

onready var color_test = $ColorTest
var color_texture = ImageTexture.new()

onready var depth_test = $DepthTest
var depth_texture = ImageTexture.new()

onready var depth_space_test = $DepthSpaceTest
var depth_space_texture = ImageTexture.new()

var timer = 0.0

func _ready():
	kinect = load("res://bin/KinectV2.gdns").new()
	color_test.texture = color_texture
	depth_test.texture = depth_texture
	depth_space_test.texture = depth_space_texture

func _process(delta):
	timer += delta
	if timer > 1:
		print("fps: " + str(Engine.get_frames_per_second()))
		timer = 0.0
	
	kinect.update()
	color_texture.create_from_image(kinect.get_color_image())
	depth_texture.create_from_image(kinect.get_depth_image())
	depth_space_texture.create_from_image(kinect.get_depth_space_index_image())
