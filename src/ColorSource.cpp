#include "ColorSource.h"

using namespace godot;

void ColorSource::_register_methods() {
    register_method("_process", &ColorSource::_process);
    register_method("update", &ColorSource::update);
    register_method("get_buffer_width", &ColorSource::get_buffer_width);
    register_method("get_buffer_height", &ColorSource::get_buffer_height);
    register_method("get_image", &ColorSource::get_image);
    register_method("get_data", &ColorSource::get_data);
}

ColorSource::ColorSource() {
}

ColorSource::~ColorSource() {
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

void ColorSource::_init() {
    auto ret = GetDefaultKinectSensor(&_sensor);
    if (ret != S_OK) {
        return;
    }

    BOOLEAN is_open = false;
    _sensor->get_IsOpen(&is_open);
    if (!is_open) {
        auto ret = _sensor->Open();
    }

    IColorFrameSource* color_frame_source = nullptr;
    ret = _sensor->get_ColorFrameSource(&color_frame_source);
    ret = color_frame_source->OpenReader(&_reader);

    IFrameDescription* frame_desc;
    ret = color_frame_source->CreateFrameDescription(ColorImageFormat_Rgba, &frame_desc);

    unsigned int bpp = 0;
    ret = frame_desc->get_BytesPerPixel(&bpp);
    unsigned int lip = 0;
    ret = frame_desc->get_LengthInPixels(&lip);

    _data.resize(bpp * lip);

    frame_desc->Release();
    color_frame_source->Release();

    if (_image.is_null()) {
        _image.instance();
    }
}

void ColorSource::_process(float delta) {
    update();
}

bool ColorSource::update() {
    if (_reader == nullptr) {
        return false;
    }

    IColorFrame* frame = nullptr;
    auto ret = _reader->AcquireLatestFrame(&frame);
    if (FAILED(ret)) {
        if (frame != nullptr) {
            frame->Release();
            frame = nullptr;
        }
        return false;
    }

    frame->CopyConvertedFrameDataToArray(_data.size(), _data.write().ptr(), ColorImageFormat_Rgba);

    _image->create_from_data(1920, 1080, false, Image::FORMAT_RGBA8, _data);

    if (frame != nullptr) {
        frame->Release();
        frame = nullptr;
    }

    return true;
}
