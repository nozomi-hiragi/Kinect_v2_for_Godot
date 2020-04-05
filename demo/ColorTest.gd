extends Sprite

var color_source
var image_texture = ImageTexture.new()
var image = Image.new()

func _ready():
	Engine.target_fps = 30
	color_source = load("res://bin/ColorSource.gdns").new()
	texture = image_texture

func _process(delta):
	if !color_source.update():
		return

	var color_image = color_source.get_image()
	if color_image == null:
		return

	image_texture.create_from_image(color_image)
