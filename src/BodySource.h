#ifndef _BODY_SOURCE_H_
#define _BODY_SOURCE_H_

#include <Godot.hpp>
#include <ImageTexture.hpp>
#include <Kinect.h>
#include "KinectSensorWrap.h"
#include "BodyFrameSourceWrap.h"
#include "CoordinateMapperWrap.h"

namespace godot {

class BodySource : public Reference {
    GODOT_CLASS(BodySource, Reference)

private:
    IBody* _bodies[BODY_COUNT];
    BOOLEAN _is_tracked[BODY_COUNT];
    Joint _joints[BODY_COUNT][JointType_Count];
    JointOrientation _joint_orientations[BODY_COUNT][JointType_Count];

    KinectSensorWrap _kinect_sensor;
    std::unique_ptr<BodyFrameSourceWrap> _body_frame_source;
    std::unique_ptr<CoordinatorMapperWrap> _coordinator_mapper;

public:
    static void _register_methods();

    BodySource();
    ~BodySource();

    void _init(); // our initializer called by Godot
    void _process(float delta);

    bool update();

    bool is_tracked(int index) {
        if (unsigned short(index) >= BODY_COUNT) {
            return false;
        }
        return _is_tracked[index];
    }

    int get_tracking_state(int body_index, int joint_index) {
        if (unsigned short(body_index) >= BODY_COUNT) {
            return TrackingState_NotTracked;
        }
        if (unsigned short(joint_index) >= JointType_Count) {
            return TrackingState_NotTracked;
        }
        return _joints[body_index][joint_index].TrackingState;
    }

    Vector3 get_joint_camera_position(int body_index, int joint_index) {
        if (unsigned short(body_index) >= BODY_COUNT) {
            return Vector3();
        }
        if (unsigned short(joint_index) >= JointType_Count) {
            return Vector3();
        }
        auto pos = _joints[body_index][joint_index].Position;
        return Vector3(pos.X, pos.Y, pos.Z);
    }

    Vector2 get_joint_color_position(int body_index, int joint_index) {
        if (unsigned short(body_index) >= BODY_COUNT) {
            return Vector2();
        }
        if (unsigned short(joint_index) >= JointType_Count) {
            return Vector2();
        }
        ColorSpacePoint pos;
        _coordinator_mapper->kari()->MapCameraPointToColorSpace(_joints[body_index][joint_index].Position, &pos);
        return Vector2(pos.X, pos.Y);
    }

    Vector2 get_joint_depth_position(int body_index, int joint_index) {
        if (unsigned short(body_index) >= BODY_COUNT) {
            return Vector2();
        }
        if (unsigned short(joint_index) >= JointType_Count) {
            return Vector2();
        }
        DepthSpacePoint pos;
        _coordinator_mapper->kari()->MapCameraPointToDepthSpace(_joints[body_index][joint_index].Position, &pos);
        return Vector2(pos.X, pos.Y);
    }

    Quat get_joint_orientation(int body_index, int joint_index) {
        if (unsigned short(body_index) >= BODY_COUNT) {
            return Quat();
        }
        if (unsigned short(joint_index) >= JointType_Count) {
            return Quat();
        }
        auto pos = _joint_orientations[body_index][joint_index].Orientation;
        return Quat(pos.x, pos.y, pos.z, pos.w);
    }
};

}

#endif
