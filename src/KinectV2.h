#ifndef _KINECT_V2_H_
#define _KINECT_V2_H_

#include <Godot.hpp>
#include <Image.hpp>
#include <vector>
#include "KinectSensorWrap.h"
#include "ColorFrameSourceWrap.h"
#include "DepthFrameSourceWrap.h"
#include "BodyIndexFrameSourceWrap.h"
#include "BodyFrameSourceWrap.h"
#include "CoordinateMapperWrap.h"

class KinectV2 : public godot::Reference {
    GODOT_CLASS(KinectV2, godot::Reference)
public:
    static void _register_methods();

    KinectV2();
    ~KinectV2();

    void _init(); // our initializer called by Godot
    void _process(float delta);
    void update();

    godot::Ref<godot::Image> get_color_image() const { return _color_image; }
    godot::Ref<godot::Image> get_depth_image() const { return _depth_image; }
    godot::Ref<godot::Image> get_depth_space_index_image() const { return _depth_space_index_image; }
    godot::Ref<godot::Image> get_color_space_index_image() const { return _color_space_index_image; }
    godot::Ref<godot::Image> get_body_index_image() const { return _body_index_image; }

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

    godot::Vector3 get_joint_camera_position(int body_index, int joint_index) {
        if (unsigned short(body_index) >= BODY_COUNT) {
            return godot::Vector3();
        }
        if (unsigned short(joint_index) >= JointType_Count) {
            return godot::Vector3();
        }
        auto pos = _joints[body_index][joint_index].Position;
        return godot::Vector3(pos.X, pos.Y, pos.Z);
    }

    godot::Vector2 get_joint_color_position(int body_index, int joint_index) {
        if (unsigned short(body_index) >= BODY_COUNT) {
            return godot::Vector2();
        }
        if (unsigned short(joint_index) >= JointType_Count) {
            return godot::Vector2();
        }
        auto pos = _coordinate_mapper->cameraToColor(_joints[body_index][joint_index].Position);
        return godot::Vector2(pos.X, pos.Y);
    }

    godot::Vector2 get_joint_depth_position(int body_index, int joint_index) {
        if (unsigned short(body_index) >= BODY_COUNT) {
            return godot::Vector2();
        }
        if (unsigned short(joint_index) >= JointType_Count) {
            return godot::Vector2();
        }
        auto pos = _coordinate_mapper->cameraToDepth(_joints[body_index][joint_index].Position);
        return godot::Vector2(pos.X, pos.Y);
    }

    godot::Quat get_joint_orientation(int body_index, int joint_index) {
        if (unsigned short(body_index) >= BODY_COUNT) {
            return godot::Quat();
        }
        if (unsigned short(joint_index) >= JointType_Count) {
            return godot::Quat();
        }
        auto pos = _joint_orientations[body_index][joint_index].Orientation;
        return godot::Quat(pos.x, pos.y, pos.z, pos.w);
    }

private:
    KinectSensorWrap _kinect_sensor;
    std::unique_ptr<ColorFrameSourceWrap> _color_frame_source;
    std::unique_ptr<DepthFrameSourceWrap> _depth_frame_source;
    std::unique_ptr<BodyIndexFrameSourceWrap> _body_index_frame_source;
    std::unique_ptr<BodyFrameSourceWrap> _body_frame_source;
    std::unique_ptr<CoordinateMapperWrap> _coordinate_mapper;

    godot::PoolByteArray _color_data;
    godot::Ref<godot::Image> _color_image;

    std::vector<UINT16> _depth_data_buffer;
    godot::PoolIntArray _depth_data_raw;
    godot::PoolByteArray _depth_data;
    godot::Ref<godot::Image> _depth_image;

    std::vector<DepthSpacePoint> _depth_space_point_buffer;
    godot::PoolByteArray _depth_space_points;
    godot::Ref<godot::Image> _depth_space_index_image;

    std::vector<ColorSpacePoint> _color_space_point_buffer;
    godot::PoolByteArray _color_space_points;
    godot::Ref<godot::Image> _color_space_index_image;

    godot::PoolByteArray _body_index_data;
    godot::Ref<godot::Image> _body_index_image;

    IBody* _bodies[BODY_COUNT];
    BOOLEAN _is_tracked[BODY_COUNT];
    Joint _joints[BODY_COUNT][JointType_Count];
    JointOrientation _joint_orientations[BODY_COUNT][JointType_Count];

    bool ticktack;
};

#endif
