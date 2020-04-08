#include "DepthSource.h"
#include <string>
#include <ppl.h>

using namespace godot;

void DepthSource::_register_methods() {
    register_method("_process", &DepthSource::_process);
    register_method("update", &DepthSource::update);
    register_method("get_buffer_width", &DepthSource::get_buffer_width);
    register_method("get_buffer_height", &DepthSource::get_buffer_height);
    register_method("get_data", &DepthSource::get_data);
    register_method("get_image", &DepthSource::get_image);
    register_method("get_color_stape_index_image", &DepthSource::get_color_stape_index_image);
}

DepthSource::DepthSource() {
}

DepthSource::~DepthSource() {
    _coordinator_mapper->release();
    _depth_frame_source->release();
    _kinect_sensor.release();
}

void DepthSource::_init() {
    _kinect_sensor.init();

    _depth_frame_source = _kinect_sensor.create<DepthFrameSourceWrap>();
    _coordinator_mapper = _kinect_sensor.create<CoordinatorMapperWrap>();

    auto desc = _depth_frame_source->getFrameDescription();
    _buffer.resize(desc._length_in_pixels);
    _data.resize(desc._length_in_pixels);
    _normalize_data.resize(desc._length_in_pixels);
    _color_space_points.resize(1920*1080*2);

    if (_image.is_null()) {
        _image.instance();
    }
    _image->create_from_data(get_buffer_width(), get_buffer_height(), false, Image::FORMAT_L8, _normalize_data);

    if (_color_space_index_image.is_null()) {
        _color_space_index_image.instance();
    }
    _ticktack = true;
}

void DepthSource::_process(float delta) {
    update();
}

bool DepthSource::update() {

    auto ret = _depth_frame_source->update([&](IDepthFrame* frame) {
       frame->CopyFrameDataToArray(_buffer.size(), _buffer.data());
    });

    if (!ret) {
        return false;
    }

    // Heavy
    if (_ticktack) {
        int color_dot_count = 1920*1080;
        std::vector<DepthSpacePoint> depth_space_points(color_dot_count);
        _coordinator_mapper->kari()->MapColorFrameToDepthSpace(
            _buffer.size(),
            _buffer.data(),
            color_dot_count,
            depth_space_points.data());

        auto color_write_ptr = _color_space_points.write().ptr();
        auto depth_space_point_ptr = depth_space_points.data();
        concurrency::parallel_for(0, color_dot_count, [&](int w) {
            int base = w;

            color_write_ptr[base*2+0] = depth_space_point_ptr[base].X * 0.498046875;
            color_write_ptr[base*2+1] = depth_space_point_ptr[base].Y * 0.6014150943396226;
        });

        _color_space_index_image->create_from_data(1920, 1080, false, Image::FORMAT_RG8, _color_space_points);
    }
    _ticktack = !_ticktack;

    auto data_ptr = _data.write().ptr();
    auto normalize_data_ptr = _normalize_data.write().ptr();

    concurrency::parallel_for(0, get_buffer_width() * get_buffer_height(), [&](int i) {
        data_ptr[i] = _buffer[i];
        normalize_data_ptr[i] = 0.031875f * data_ptr[i];
    });

    _image->create_from_data(get_buffer_width(), get_buffer_height(), false, Image::FORMAT_L8, _normalize_data);

    return true;
}
