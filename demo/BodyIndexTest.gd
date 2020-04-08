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
	
	image_texture.create_from_image(body_index_source.get_image())
