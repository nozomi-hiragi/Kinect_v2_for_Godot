extends Sprite

var body_index_source
var image_texture = ImageTexture.new()
var image = Image.new()

func _ready():
	body_index_source = load("res://bin/BodyIndexSource.gdns").new()
	texture = image_texture

func _process(delta):
	if !body_index_source.update():
		return

	var body_index_data = body_index_source.get_data()
	if body_index_data == null:
		return
	var w = body_index_source.get_buffer_width()
	var h = body_index_source.get_buffer_height()

	image.create_from_data(w, h, false, Image.FORMAT_L8, PoolByteArray(Array(body_index_data)))
	image_texture.create_from_image(image)
