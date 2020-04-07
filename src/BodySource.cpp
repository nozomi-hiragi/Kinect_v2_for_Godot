#include "BodySource.h"

using namespace godot;

void BodySource::_register_methods() {
    register_method("_process", &BodySource::_process);
    register_method("update", &BodySource::update);

    register_method("is_tracked", &BodySource::is_tracked);
    register_method("get_tracking_state", &BodySource::get_tracking_state);
    register_method("get_joint_camera_position", &BodySource::get_joint_camera_position);
    register_method("get_joint_color_position", &BodySource::get_joint_color_position);
    register_method("get_joint_depth_position", &BodySource::get_joint_depth_position);
    register_method("get_joint_orientation", &BodySource::get_joint_orientation);
} 

BodySource::BodySource() {
    for (auto& body : _bodies) body = nullptr;
}

BodySource::~BodySource() {
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

void BodySource::_init() {
    auto ret = GetDefaultKinectSensor(&_sensor);
    if (ret != S_OK) {
        return;
    }

    BOOLEAN is_open = false;
    _sensor->get_IsOpen(&is_open);
    if (!is_open) {
        auto ret = _sensor->Open();
    }

    IBodyFrameSource* body_frame_source = nullptr;
    ret = _sensor->get_BodyFrameSource(&body_frame_source);
    ret = body_frame_source->OpenReader(&_reader);

    _sensor->get_CoordinateMapper(&_mapper);

    body_frame_source->Release();
}

void BodySource::_process(float delta) {
    update();
}

bool BodySource::update() {
    if (_reader == nullptr) {
        return false;
    }

    IBodyFrame* frame = nullptr;
    auto ret = _reader->AcquireLatestFrame(&frame);
    if (FAILED(ret)) {
        if (frame != nullptr) {
            frame->Release();
            frame = nullptr;
        }
        return false;
    }

    for (auto& body : _bodies) {
        if (body == nullptr) {
            continue;
        }
        body->Release();
        body = nullptr;
    }

    frame->GetAndRefreshBodyData(BODY_COUNT, _bodies);

    if (frame != nullptr) {
        frame->Release();
        frame = nullptr;
    }

    for (int i = 0; i < BODY_COUNT; i++) {
        _bodies[i]->get_IsTracked(&_is_tracked[i]);
        if (!_is_tracked[i]) {
            continue;
        }
        _bodies[i]->GetJoints(JointType_Count, _joints[i]);
        _bodies[i]->GetJointOrientations(JointType_Count, _joint_orientations[i]);
    }

    return true;
}
