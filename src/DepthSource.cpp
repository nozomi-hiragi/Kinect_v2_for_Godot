#include "DepthSource.h"
#include <string>

using namespace godot;

void DepthSource::_register_methods() {
    register_method("_process", &DepthSource::_process);
    register_method("update", &DepthSource::update);
    register_method("get_depth_width", &DepthSource::get_depth_width);
    register_method("get_depth_height", &DepthSource::get_depth_height);
    register_method("get_data", &DepthSource::get_data);
} 

DepthSource::DepthSource() {
}

DepthSource::~DepthSource() {
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
    ret = frame_desc->get_Width(&_depth_width);
    ret = frame_desc->get_Height(&_depth_height);

    unsigned int lip = 0;
    ret = frame_desc->get_LengthInPixels(&lip);

    _buffer.resize(lip);
    _data.resize(lip);

    frame_desc->Release();
    depth_frame_source->Release();
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

    auto data_ptr = _data.write().ptr();
    for (int i = 0; i < _buffer.size(); i++) {
        data_ptr[i] = _buffer[i];
    }

    if (frame != nullptr) {
        frame->Release();
        frame = nullptr;
    }

    return true;
}
