#ifndef _DEPTH_SOURCE_H_
#define _DEPTH_SOURCE_H_

#include <vector>
#include <Godot.hpp>
#include <ImageTexture.hpp>
#include <Kinect.h>
#include "KinectSensorWrap.h"
#include "CoordinateMapperWrap.h"

namespace godot {

class DepthSource : public Reference {
    GODOT_CLASS(DepthSource, Reference)

private:
    std::vector<UINT16> _buffer;
    PoolIntArray _data;
    PoolByteArray _normalize_data;
    Ref<Image> _image;

    PoolByteArray _color_space_points;
    Ref<Image> _color_space_index_image;

    KinectSensorWrap _kinect_sensor;
    std::unique_ptr<DepthFrameSourceWrap> _depth_frame_source;
    std::unique_ptr<CoordinatorMapperWrap> _coordinator_mapper;

    bool _ticktack;

public:
    static void _register_methods();

    DepthSource();
    ~DepthSource();

    void _init(); // our initializer called by Godot
    void _process(float delta);

    bool update();
    inline int get_buffer_width() { return 512; }
    inline int get_buffer_height() { return 424; }
    Ref<Image> get_image() { return _image; }
    PoolIntArray get_data() { return _data; }
    Ref<Image> get_color_stape_index_image() { return _color_space_index_image; }
};

}

#endif
