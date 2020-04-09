#include "KinectV2.h"
#include <ppl.h>

const int COLOR_WIDTH = 1920;
const int COLOR_HEIGHT = 1080;
const int COLOR_DOTS = COLOR_WIDTH * COLOR_HEIGHT;
const int DEPTH_WIDTH = 512;
const int DEPTH_HEIGHT = 424;
const int DEPTH_DOTS = DEPTH_WIDTH * DEPTH_HEIGHT;
const float DEPTH_NORMALIZE = 255.f / 8000.f;

using namespace godot;

void KinectV2::_register_methods() {
    register_method("_process", &KinectV2::_process);
    register_method("update", &KinectV2::update);
    register_method("get_color_image", &KinectV2::get_color_image);
    register_method("get_depth_image", &KinectV2::get_depth_image);
    register_method("get_depth_space_index_image", &KinectV2::get_depth_space_index_image);
    register_method("get_color_space_index_image", &KinectV2::get_color_space_index_image);
    register_method("get_body_index_image", &KinectV2::get_body_index_image);
    register_method("is_tracked", &KinectV2::is_tracked);
    register_method("get_tracking_state", &KinectV2::get_tracking_state);
    register_method("get_joint_camera_position", &KinectV2::get_joint_camera_position);
    register_method("get_joint_color_position", &KinectV2::get_joint_color_position);
    register_method("get_joint_depth_position", &KinectV2::get_joint_depth_position);
    register_method("get_joint_orientation", &KinectV2::get_joint_orientation);
}

KinectV2::KinectV2() {
    ticktack = true;
    for (auto& body : _bodies) body = nullptr;
}

KinectV2::~KinectV2() {
    _coordinate_mapper->release();
    _body_frame_source->release();
    _body_index_frame_source->release();
    _depth_frame_source->release();
    _color_frame_source->release();
    _kinect_sensor.release();
}

void KinectV2::_init() {
    _kinect_sensor.init();
    _color_frame_source = _kinect_sensor.create<ColorFrameSourceWrap>();
    _depth_frame_source = _kinect_sensor.create<DepthFrameSourceWrap>();
    _body_index_frame_source = _kinect_sensor.create<BodyIndexFrameSourceWrap>();
    _body_frame_source = _kinect_sensor.create<BodyFrameSourceWrap>();
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
    _depth_space_point_buffer.resize(color_desc._length_in_pixels);
    if (_depth_space_index_image.is_null()) {
        _depth_space_index_image.instance();
        _depth_space_index_image->create_from_data(COLOR_WIDTH, COLOR_HEIGHT, false, Image::FORMAT_RG8, _depth_space_points);
    }

    _color_space_points.resize(depth_desc._length_in_pixels * 2);
    _color_space_point_buffer.resize(depth_desc._length_in_pixels);
    if (_color_space_index_image.is_null()) {
        _color_space_index_image.instance();
        _color_space_index_image->create_from_data(DEPTH_WIDTH, DEPTH_HEIGHT, false, Image::FORMAT_RG8, _color_space_points);
    }

    _body_index_data.resize(depth_desc._length_in_pixels);
    if (_body_index_image.is_null()) {
        _body_index_image.instance();
        _body_index_image->create_from_data(DEPTH_WIDTH, DEPTH_HEIGHT, false, Image::FORMAT_L8, _body_index_data);
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

    auto is_update_body_index = _body_index_frame_source->update([&](IBodyIndexFrame* frame) {
        frame->CopyFrameDataToArray(_body_index_data.size(), _body_index_data.write().ptr());
       _body_index_image->create_from_data(DEPTH_WIDTH, DEPTH_HEIGHT, false, Image::FORMAT_L8, _body_index_data);
    });

    auto is_update_body = _body_frame_source->update([&](IBodyFrame* frame) {
        for (auto& body : _bodies) {
            if (body == nullptr) {
                continue;
            }
            body->Release();
            body = nullptr;
        }
        frame->GetAndRefreshBodyData(BODY_COUNT, _bodies);
        concurrency::parallel_for(0, BODY_COUNT, [&](int i) {
            _bodies[i]->get_IsTracked(&_is_tracked[i]);
            if (_is_tracked[i]) {
                _bodies[i]->GetJoints(JointType_Count, _joints[i]);
                _bodies[i]->GetJointOrientations(JointType_Count, _joint_orientations[i]);
            }
        });
    });

    if (is_update_depth) {
        if (ticktack) {
            _coordinate_mapper->colorFrameToDepth(_depth_data_buffer.data(), _depth_space_point_buffer.data());
        } else {
            _coordinate_mapper->depthFrameToColor(_depth_data_buffer.data(), _color_space_point_buffer.data());
        }

        auto depth_data_raw_ptr = _depth_data_raw.write().ptr();
        auto depth_data_ptr = _depth_data.write().ptr();

        auto depth_space_from = _depth_space_point_buffer.data();
        auto depth_space_to = _depth_space_points.write().ptr();

        auto color_space_from = _color_space_point_buffer.data();
        auto color_space_to = _color_space_points.write().ptr();

        concurrency::parallel_for(0, DEPTH_DOTS, [&](int i) {
            // normalize depth
            depth_data_raw_ptr[i] = _depth_data_buffer[i];
            depth_data_ptr[i] = depth_data_raw_ptr[i] * DEPTH_NORMALIZE;

            if (ticktack) {
                depth_space_to[i*2+0] = depth_space_from[i].X * 0.498046875;
                depth_space_to[i*2+1] = depth_space_from[i].Y * 0.6014150943396226;
            }else{
                color_space_to[i*2+0] = color_space_from[i].X * (255.f/COLOR_WIDTH);
                color_space_to[i*2+1] = color_space_from[i].Y * (255.f/COLOR_HEIGHT);
            }
        });
        if (ticktack) {
            concurrency::parallel_for(DEPTH_DOTS, COLOR_DOTS, [&](int base) {
                depth_space_to[base*2+0] = depth_space_from[base].X * 0.498046875;
                depth_space_to[base*2+1] = depth_space_from[base].Y * 0.6014150943396226;
            });
            _depth_space_index_image->create_from_data(COLOR_WIDTH, COLOR_HEIGHT, false, Image::FORMAT_RG8, _depth_space_points);
        } else {
            _color_space_index_image->create_from_data(DEPTH_WIDTH, DEPTH_HEIGHT, false, Image::FORMAT_RG8, _color_space_points);
        }
        ticktack = !ticktack;

        _depth_image->create_from_data(DEPTH_WIDTH, DEPTH_HEIGHT, false, Image::FORMAT_L8, _depth_data);
    }
}
