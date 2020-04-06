extends Sprite

var depth_source
var image_texture = ImageTexture.new()
var image = Image.new()

func _ready():
	depth_source = load("res://bin/DepthSource.gdns").new()
	texture = image_texture
	
func _process(delta):
	if !depth_source.update():
		return
	
	var depth_data = depth_source.get_data()
	if depth_data == null:
		return
	var w = depth_source.get_buffer_width()
	var h = depth_source.get_buffer_height()

	image.create_from_data(w, h, false, Image.FORMAT_L8, PoolByteArray(Array(depth_data)))
	image_texture.create_from_image(image)
