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

public:
    static void _register_methods();

    ColorSource();
    ~ColorSource();

    void _init(); // our initializer called by Godot
    void _process(float delta);

    bool update();
    inline int get_buffer_width() { return 1920; }
    inline int get_buffer_height() { return 1080; }
    Ref<Image> get_image() { return _image; }
    PoolByteArray get_data() { return _data; }
};

}

#endif
