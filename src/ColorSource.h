#ifndef _COLOR_SOURCE_H_
#define _COLOR_SOURCE_H_

#include <Godot.hpp>
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

    int _buffer_width;
    int _buffer_height;

public:
    static void _register_methods();

    ColorSource();
    ~ColorSource();

    void _init(); // our initializer called by Godot
    void _process(float delta);

    bool update();
    int get_buffer_width() { return _buffer_width; }
    int get_buffer_height() { return _buffer_height; }
    Ref<Image> get_image() { return _image; }
    PoolByteArray get_data() { return _data; }
};

}

#endif
