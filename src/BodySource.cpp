#include "BodySource.h"
#include <ppl.h>

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
    _coordinator_mapper->release();
    _body_frame_source->release();
    _kinect_sensor.release();
}

void BodySource::_init() {
    _kinect_sensor.init();
    _body_frame_source = _kinect_sensor.create<BodyFrameSourceWrap>();
    _coordinator_mapper = _kinect_sensor.create<CoordinatorMapperWrap>();
}

void BodySource::_process(float delta) {
    update();
}

bool BodySource::update() {
    auto ret = _body_frame_source->update([&](IBodyFrame* frame) {
        for (auto& body : _bodies) {
            if (body == nullptr) {
                continue;
            }
            body->Release();
            body = nullptr;
        }

        frame->GetAndRefreshBodyData(BODY_COUNT, _bodies);
    });

    if (!ret) {
        return false;
    }

    concurrency::parallel_for(0, BODY_COUNT, [&](int i) {
        _bodies[i]->get_IsTracked(&_is_tracked[i]);
        if (_is_tracked[i]) {
            _bodies[i]->GetJoints(JointType_Count, _joints[i]);
            _bodies[i]->GetJointOrientations(JointType_Count, _joint_orientations[i]);
        }
    });

    return true;
}
