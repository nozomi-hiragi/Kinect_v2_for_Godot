#include "ColorSource.h"

using namespace godot;

void ColorSource::_register_methods() {
    register_method("_process", &ColorSource::_process);
    register_method("update", &ColorSource::update);
    register_method("get_color_width", &ColorSource::get_color_width);
    register_method("get_color_height", &ColorSource::get_color_height);
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
    ret = frame_desc->get_Width(&_color_width);
    ret = frame_desc->get_Height(&_color_height);

    unsigned int bpp = 0;
    ret = frame_desc->get_BytesPerPixel(&bpp);
    unsigned int lip = 0;
    ret = frame_desc->get_LengthInPixels(&lip);

    _data.resize(bpp * lip);

    frame_desc->Release();
    color_frame_source->Release();
}

void ColorSource::_process(float delta) {
    update();
}

void ColorSource::update() {
    if (_reader == nullptr) {
        return;
    }

    IColorFrame* frame = nullptr;
    auto ret = _reader->AcquireLatestFrame(&frame);
    if (FAILED(ret)) {
        if (frame != nullptr) {
            frame->Release();
            frame = nullptr;
        }
        return;
    }

    frame->CopyConvertedFrameDataToArray(_data.size(), _data.write().ptr(), ColorImageFormat_Rgba);

    if (_image.is_null()) {
        _image.instance();
    }
    _image->create_from_data(_color_width, _color_height, false, Image::FORMAT_RGBA8, _data);

    if (frame != nullptr) {
        frame->Release();
        frame = nullptr;
    }
}
