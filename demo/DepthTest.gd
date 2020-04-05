extends Sprite

var depth_source
var image_texture = ImageTexture.new()
var image = Image.new()
var thread
var is_alive = true

func _ready():
	depth_source = load("res://bin/DepthSource.gdns").new()
	texture = image_texture
	
	thread = Thread.new()
#	thread.start(self, "_update")

func _update(userdata):
	while is_alive:
		depth_source.update()

func _process(delta):
	if !depth_source.update():
		return
	
	var depth_data = depth_source.get_data()
	if depth_data == null:
		return
	var w = depth_source.get_depth_width()
	var h = depth_source.get_depth_height()
	var color_data = PoolByteArray()
	var mi = 65535
	var ma = 0
	for i in range(depth_data.size()):
#		mi = min(mi, depth_data[i])
#		ma = max(ma, depth_data[i])
		color_data.push_back(depth_data[i] * 0.031875)
#		color_data.push_back(depth_data[i]%255)
		color_data.push_back(0)
		color_data.push_back(0)
#		color_data.push_back(255)

	image.create_from_data(w, h, false, Image.FORMAT_RGB8, color_data)
	image_texture.create_from_image(image)

func _exit_tree():
	is_alive = false
	thread.wait_to_finish()
