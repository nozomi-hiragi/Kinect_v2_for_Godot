#ifndef _DEPTH_SOURCE_H_
#define _DEPTH_SOURCE_H_

#include <vector>
#include <Godot.hpp>
#include <ImageTexture.hpp>
#include <Kinect.h>

namespace godot {

class DepthSource : public Reference {
    GODOT_CLASS(DepthSource, Reference)

private:
    IKinectSensor* _sensor;
    IDepthFrameReader* _reader;
    std::vector<UINT16> _buffer;
    PoolIntArray _data;

    int _buffer_width;
    int _buffer_height;

public:
    static void _register_methods();

    DepthSource();
    ~DepthSource();

    void _init(); // our initializer called by Godot
    void _process(float delta);

    bool update();
    int get_buffer_width() { return _buffer_width; }
    int get_buffer_height() { return _buffer_height; }
    PoolIntArray get_data() { return _data; }
};

}

#endif
