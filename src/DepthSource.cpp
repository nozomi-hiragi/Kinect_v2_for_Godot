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
    if (_mapper) {
        _mapper->Release();
        _mapper = nullptr;
    }

    if (_reader) {
        _reader->Release();
        _reader = nullptr;
    }

    if (_sensor) {
        BOOLEAN is_open = false;
        _sensor->get_IsOpen(&is_open);
        if (is_open) {
            _sensor->Close();
        }
        _sensor->Release();
        _sensor = nullptr;
    }
}

void DepthSource::_init() {
    auto ret = GetDefaultKinectSensor(&_sensor);
    if (ret != S_OK) {
        return;
    }

    BOOLEAN is_open = false;
    _sensor->get_IsOpen(&is_open);
    if (!is_open) {
        auto ret = _sensor->Open();
    }

    IDepthFrameSource* depth_frame_source = nullptr;
    ret = _sensor->get_DepthFrameSource(&depth_frame_source);
    ret = depth_frame_source->OpenReader(&_reader);

    IFrameDescription* frame_desc;
    ret = depth_frame_source->get_FrameDescription(&frame_desc);

    unsigned int lip = 0;
    ret = frame_desc->get_LengthInPixels(&lip);

    _buffer.resize(lip);
    _data.resize(lip);
    _normalize_data.resize(lip);

    _color_space_points.resize(1920*1080*2);

    _sensor->get_CoordinateMapper(&_mapper);

    frame_desc->Release();
    depth_frame_source->Release();

    if (_image.is_null()) {
        _image.instance();
    }

    if (_color_space_index_image.is_null()) {
        _color_space_index_image.instance();
    }
    _ticktack = true;
}

void DepthSource::_process(float delta) {
    update();
}

bool DepthSource::update() {
    if (_reader == nullptr) {
        return false;
    }

    IDepthFrame* frame = nullptr;
    auto ret = _reader->AcquireLatestFrame(&frame);
    if (FAILED(ret)) {
        if (frame != nullptr) {
            frame->Release();
            frame = nullptr;
        }
        return false;
    }

    frame->CopyFrameDataToArray(_buffer.size(), _buffer.data());


    // Heavy
    if (_ticktack) {
        int color_dot_count = 1920*1080;
        std::vector<DepthSpacePoint> depth_space_points(color_dot_count);
        _mapper->MapColorFrameToDepthSpace(
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

    if (frame != nullptr) {
        frame->Release();
        frame = nullptr;
    }

    return true;
}
