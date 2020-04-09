extends Spatial

onready var mesh_instance = $MeshInstance
var kinect
var mat
var color_texture = ImageTexture.new()
var depth_texture = ImageTexture.new()
var body_idx_texture = ImageTexture.new()
var depth_space_texture = ImageTexture.new()
var color_space_texture = ImageTexture.new()

func _ready():
	kinect = load("res://bin/KinectV2.gdns").new()
	
	mat = mesh_instance.get_surface_material(0) as ShaderMaterial
	mat.set_shader_param("ColorTexture", color_texture)
	mat.set_shader_param("DepthTexture", depth_texture)
	mat.set_shader_param("BodyIndexTexture", body_idx_texture)
	mat.set_shader_param("DepthSpaceTexture", depth_space_texture)
	mat.set_shader_param("ColorSpaceTexture", color_space_texture)
	mat.set_shader_param("AvailableDepth", 0.89)
	

func _process(delta):
	kinect.update()
	color_texture.create_from_image(kinect.get_color_image())
	depth_texture.create_from_image(kinect.get_depth_image())
	body_idx_texture.create_from_image(kinect.get_body_index_image())
	depth_space_texture.create_from_image(kinect.get_depth_space_index_image())
	color_space_texture.create_from_image(kinect.get_color_space_index_image())
	
