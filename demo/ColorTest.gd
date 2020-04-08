extends Sprite

var color_source
var image_texture = ImageTexture.new()

func _ready():
	color_source = load("res://bin/ColorSource.gdns").new()
	texture = image_texture

func _process(delta):
	if !color_source.update():
		return

	var color_image = color_source.get_image()
	if color_image == null:
		return

	image_texture.create_from_image(color_image)
	
	timer += delta
	if timer > TIMER_LIMIT: # Prints every 2 seconds
		timer = 0.0
		print("fps: " + str(Engine.get_frames_per_second()))
const TIMER_LIMIT = 1
var timer = 0.0
