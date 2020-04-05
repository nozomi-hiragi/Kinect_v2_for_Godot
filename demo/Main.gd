extends Node

# Declare member variables here. Examples:
# var a = 2
# var b = "text"

onready var sprite = $Sprite

var color_source
var image_texture = ImageTexture.new()

# Called when the node enters the scene tree for the first time.
func _ready():
	color_source = load("res://bin/ColorSource.gdns").new()
	sprite.texture = image_texture

func _process(delta):
	color_source.update()
	
	var color_image = color_source.get_image()
	if color_image == null:
		return
	image_texture.create_from_image(color_image)
