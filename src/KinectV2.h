#ifndef _KINECT_V2_H_
#define _KINECT_V2_H_

#include <Godot.hpp>
#include <Image.hpp>
#include <vector>
#include "KinectSensorWrap.h"
#include "ColorFrameSourceWrap.h"
#include "DepthFrameSourceWrap.h"
#include "CoordinateMapperWrap.h"

class KinectV2 : public godot::Reference {
    GODOT_CLASS(KinectV2, godot::Reference)
public:
    static void _register_methods();

    KinectV2();
    ~KinectV2();

    void _init(); // our initializer called by Godot
    void _process(float delta);
    void update();

    godot::Ref<godot::Image> get_color_image() const { return _color_image; }
    godot::Ref<godot::Image> get_depth_image() const { return _depth_image; }
    godot::Ref<godot::Image> get_depth_space_index_image() const { return _depth_space_index_image; }

private:
    KinectSensorWrap _kinect_sensor;
    std::unique_ptr<ColorFrameSourceWrap> _color_frame_source;
    std::unique_ptr<DepthFrameSourceWrap> _depth_frame_source;
    std::unique_ptr<CoordinateMapperWrap> _coordinate_mapper;

    godot::PoolByteArray _color_data;
    godot::Ref<godot::Image> _color_image;

    std::vector<UINT16> _depth_data_buffer;
    godot::PoolIntArray _depth_data_raw;
    godot::PoolByteArray _depth_data;
    godot::Ref<godot::Image> _depth_image;

    godot::PoolByteArray _depth_space_points;
    godot::Ref<godot::Image> _depth_space_index_image;

    bool kari;
};

#endif
