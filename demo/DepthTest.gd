extends Sprite

var depth_source
var image_texture = ImageTexture.new()

onready var depth_space_test = $"../DepthSpaceTest"
var depth_space_image_texture = ImageTexture.new()

func _ready():
	depth_source = load("res://bin/DepthSource.gdns").new()
	texture = image_texture
	
	depth_space_test.texture = depth_space_image_texture
	
func _process(delta):
	if !depth_source.update():
		return
	
	image_texture.create_from_image(depth_source.get_image())
	
	depth_space_image_texture.create_from_image(depth_source.get_color_stape_index_image())
