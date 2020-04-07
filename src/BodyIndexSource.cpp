#include "BodyIndexSource.h"

using namespace godot;

void BodyIndexSource::_register_methods() {
    register_method("_process", &BodyIndexSource::_process);
    register_method("update", &BodyIndexSource::update);
    register_method("get_buffer_width", &BodyIndexSource::get_buffer_width);
    register_method("get_buffer_height", &BodyIndexSource::get_buffer_height);
    register_method("get_data", &BodyIndexSource::get_data);
} 

BodyIndexSource::BodyIndexSource() {
}

BodyIndexSource::~BodyIndexSource() {
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

void BodyIndexSource::_init() {
    auto ret = GetDefaultKinectSensor(&_sensor);
    if (ret != S_OK) {
        return;
    }

    BOOLEAN is_open = false;
    _sensor->get_IsOpen(&is_open);
    if (!is_open) {
        auto ret = _sensor->Open();
    }

    IBodyIndexFrameSource* body_index_frame_source = nullptr;
    ret = _sensor->get_BodyIndexFrameSource(&body_index_frame_source);
    ret = body_index_frame_source->OpenReader(&_reader);

    IFrameDescription* frame_desc;
    ret = body_index_frame_source->get_FrameDescription(&frame_desc);
    ret = frame_desc->get_Width(&_buffer_width);
    ret = frame_desc->get_Height(&_buffer_height);

    unsigned int bpp = 0;
    ret = frame_desc->get_BytesPerPixel(&bpp);
    unsigned int lip = 0;
    ret = frame_desc->get_LengthInPixels(&lip);

    _data.resize(bpp * lip);

    frame_desc->Release();
    body_index_frame_source->Release();
}

void BodyIndexSource::_process(float delta) {
    update();
}

bool BodyIndexSource::update() {
    if (_reader == nullptr) {
        return false;
    }

    IBodyIndexFrame* frame = nullptr;
    auto ret = _reader->AcquireLatestFrame(&frame);
    if (FAILED(ret)) {
        if (frame != nullptr) {
            frame->Release();
            frame = nullptr;
        }
        return false;
    }

    frame->CopyFrameDataToArray(_data.size(), _data.write().ptr());

    if (frame != nullptr) {
        frame->Release();
        frame = nullptr;
    }

    return true;
}
