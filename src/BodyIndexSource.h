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

    int _body_index_width;
    int _body_index_height;

public:
    static void _register_methods();

    BodyIndexSource();
    ~BodyIndexSource();

    void _init(); // our initializer called by Godot
    void _process(float delta);

    bool update();
    int get_body_index_width() { return _body_index_width; }
    int get_body_index_height() { return _body_index_height; }
    PoolByteArray get_data() { return _data; }
};

}

#endif
