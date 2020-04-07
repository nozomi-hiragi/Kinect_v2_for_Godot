#ifndef _BODY_INDEX_SOURCE_H_
#define _BODY_INDEX_SOURCE_H_

#include <Godot.hpp>
#include <ImageTexture.hpp>
#include <Kinect.h>

namespace godot {

class BodyIndexSource : public Reference {
    GODOT_CLASS(BodyIndexSource, Reference)

private:
    IKinectSensor* _sensor;
    IBodyIndexFrameReader* _reader;
    PoolByteArray _data;

    int _buffer_width;
    int _buffer_height;

public:
    static void _register_methods();

    BodyIndexSource();
    ~BodyIndexSource();

    void _init(); // our initializer called by Godot
    void _process(float delta);

    bool update();
    int get_buffer_width() { return _buffer_width; }
    int get_buffer_height() { return _buffer_height; }
    PoolByteArray get_data() { return _data; }
};

}

#endif
