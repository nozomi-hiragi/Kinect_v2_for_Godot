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
    _color_frame_source->release();
    _kinect_sensor.release();
}

void ColorSource::_init() {
    _kinect_sensor.init();

    _color_frame_source = _kinect_sensor.create<ColorFrameSourceWrap>();

    auto& desc = _color_frame_source->getFrameDescription();
    _data.resize(desc._bytes_per_pixel * desc._length_in_pixels);

    if (_image.is_null()) {
        _image.instance();
    }
}

void ColorSource::_process(float delta) {
    update();
}

bool ColorSource::update() {
    auto ret = _color_frame_source->update([&](IColorFrame* color_frame){
        color_frame->CopyConvertedFrameDataToArray(_data.size(), _data.write().ptr(), ColorImageFormat_Rgba);
        _image->create_from_data(1920, 1080, false, Image::FORMAT_RGBA8, _data);
    });

    return ret;
}
