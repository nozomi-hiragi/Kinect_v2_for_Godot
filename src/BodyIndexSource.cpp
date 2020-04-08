#include "BodyIndexSource.h"

using namespace godot;

void BodyIndexSource::_register_methods() {
    register_method("_process", &BodyIndexSource::_process);
    register_method("update", &BodyIndexSource::update);
    register_method("get_buffer_width", &BodyIndexSource::get_buffer_width);
    register_method("get_buffer_height", &BodyIndexSource::get_buffer_height);
    register_method("get_image", &BodyIndexSource::get_image);
    register_method("get_data", &BodyIndexSource::get_data);
}

BodyIndexSource::BodyIndexSource() {
}

BodyIndexSource::~BodyIndexSource() {
    _body_index_frame_source->release();
    _kinect_sensor.release();
}

void BodyIndexSource::_init() {
    _kinect_sensor.init();

    _body_index_frame_source = _kinect_sensor.create<BodyIndexFrameSourceWrap>();

    auto desc = _body_index_frame_source->getFrameDescription();
    _data.resize(desc._bytes_per_pixel * desc._length_in_pixels);

    if (_image.is_null()) {
        _image.instance();
    }
}

void BodyIndexSource::_process(float delta) {
    update();
}

bool BodyIndexSource::update() {
    _body_index_frame_source->update([&](IBodyIndexFrame* frame){
        frame->CopyFrameDataToArray(_data.size(), _data.write().ptr());
       _image->create_from_data(get_buffer_width(), get_buffer_height(), false, Image::FORMAT_L8, _data);
    });

    return true;
}
