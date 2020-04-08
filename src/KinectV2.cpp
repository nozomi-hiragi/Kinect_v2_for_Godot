#include "KinectV2.h"
#include <ppl.h>

const int COLOR_WIDTH = 1920;
const int COLOR_HEIGHT = 1080;
const int DEPTH_WIDTH = 512;
const int DEPTH_HEIGHT = 424;
const float DEPTH_NORMALIZE = 255.f / 8000.f;

using namespace godot;

void KinectV2::_register_methods() {
    register_method("_process", &KinectV2::_process);
    register_method("update", &KinectV2::update);
    register_method("get_color_image", &KinectV2::get_color_image);
    register_method("get_depth_image", &KinectV2::get_depth_image);
    register_method("get_depth_space_index_image", &KinectV2::get_depth_space_index_image);
}

KinectV2::KinectV2() {
    kari = true;
}

KinectV2::~KinectV2() {
    _coordinate_mapper->release();
    _depth_frame_source->release();
    _color_frame_source->release();
    _kinect_sensor.release();
}

void KinectV2::_init() {
    _kinect_sensor.init();
    _color_frame_source = _kinect_sensor.create<ColorFrameSourceWrap>();
    _depth_frame_source = _kinect_sensor.create<DepthFrameSourceWrap>();
    _coordinate_mapper = _kinect_sensor.create<CoordinateMapperWrap>();

    auto& color_desc = _color_frame_source->getFrameDescription();
    _color_data.resize(color_desc._bytes_per_pixel * color_desc._length_in_pixels);
    if (_color_image.is_null()) {
        _color_image.instance();
        _color_image->create_from_data(COLOR_WIDTH, COLOR_HEIGHT, false, Image::FORMAT_RGBA8, _color_data);
    }

    auto& depth_desc = _depth_frame_source->getFrameDescription();
    _depth_data_raw.resize(depth_desc._length_in_pixels);
    _depth_data.resize(depth_desc._length_in_pixels);
    _depth_data_buffer.resize(depth_desc._length_in_pixels);
    if (_depth_image.is_null()) {
        _depth_image.instance();
        _depth_image->create_from_data(DEPTH_WIDTH, DEPTH_HEIGHT, false, Image::FORMAT_L8, _depth_data);
    }

    _depth_space_points.resize(color_desc._length_in_pixels * 2);
    if (_depth_space_index_image.is_null()) {
        _depth_space_index_image.instance();
        _depth_space_index_image->create_from_data(COLOR_WIDTH, COLOR_HEIGHT, false, Image::FORMAT_RG8, _depth_space_points);
    }
}

void KinectV2::_process(float delta) {
}

void KinectV2::update() {
    auto is_update_color = _color_frame_source->update([&](IColorFrame* frame) {
        frame->CopyConvertedFrameDataToArray(_color_data.size(), _color_data.write().ptr(), ColorImageFormat_Rgba);
        _color_image->create_from_data(COLOR_WIDTH, COLOR_HEIGHT, false, Image::FORMAT_RGBA8, _color_data);
    });

    auto is_update_depth = _depth_frame_source->update([&](IDepthFrame* frame) {
        frame->CopyFrameDataToArray(_depth_data_buffer.size(), _depth_data_buffer.data());
    });

    if (is_update_depth) {
        auto depth_data_raw_ptr = _depth_data_raw.write().ptr();
        auto depth_data_ptr = _depth_data.write().ptr();

        concurrency::parallel_for(0, DEPTH_WIDTH * DEPTH_HEIGHT, [&](int i) {
            depth_data_raw_ptr[i] = _depth_data_buffer[i];
            depth_data_ptr[i] = depth_data_raw_ptr[i] * DEPTH_NORMALIZE;
        });

        _depth_image->create_from_data(DEPTH_WIDTH, DEPTH_HEIGHT, false, Image::FORMAT_L8, _depth_data);

        // Heavy
        if (kari) {
            int color_dot_count = COLOR_WIDTH * COLOR_HEIGHT;
            std::vector<DepthSpacePoint> depth_space_points(color_dot_count);
            _coordinate_mapper->kari()->MapColorFrameToDepthSpace(
                _depth_data_buffer.size(),
                _depth_data_buffer.data(),
                color_dot_count,
                depth_space_points.data());

            auto depth_space_from = depth_space_points.data();
            auto depth_space_to = _depth_space_points.write().ptr();
            concurrency::parallel_for(0, color_dot_count, [&](int base) {
                depth_space_to[base*2+0] = depth_space_from[base].X * 0.498046875;
                depth_space_to[base*2+1] = depth_space_from[base].Y * 0.6014150943396226;
            });

            _depth_space_index_image->create_from_data(COLOR_WIDTH, COLOR_HEIGHT, false, Image::FORMAT_RG8, _depth_space_points);
        }
        kari = !kari;
    }
}
