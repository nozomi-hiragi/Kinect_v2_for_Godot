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
    Ref<Image> _image;

public:
    static void _register_methods();

    BodyIndexSource();
    ~BodyIndexSource();

    void _init(); // our initializer called by Godot
    void _process(float delta);

    bool update();
    inline int get_buffer_width() { return 512; }
    inline int get_buffer_height() { return 424; }
    Ref<Image> get_image() { return _image; }
    PoolByteArray get_data() { return _data; }
};

}

#endif
