#ifndef _COLOR_SOURCE_H_
#define _COLOR_SOURCE_H_

#include <vector>
#include <Godot.hpp>
#include <Sprite.hpp>
#include <ImageTexture.hpp>
#include <Kinect.h>

namespace godot {

class ColorSource : public Reference {
    GODOT_CLASS(ColorSource, Reference)

private:
    IKinectSensor* _sensor;
    IColorFrameReader* _reader;
    PoolByteArray _data;
    Ref<Image> _image;

    int _color_width;
    int _color_height;

public:
    static void _register_methods();

    ColorSource();
    ~ColorSource();

    void _init(); // our initializer called by Godot
    void _process(float delta);

    void update();
    int get_color_width() { return _color_width; }
    int get_color_height() { return _color_height; }
    Ref<Image> get_image() { return _image; }
    PoolByteArray get_data() { return _data; }
};

}

#endif
