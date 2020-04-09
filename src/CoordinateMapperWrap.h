#ifndef _COORDINATE_MAPPER_WRAP_H_
#define _COORDINATE_MAPPER_WRAP_H_

#include <Kinect.h>

class CoordinateMapperWrap {
public:
    CoordinateMapperWrap() : _coordinate_mapper(nullptr) {}
    ~CoordinateMapperWrap() {}

    void init(IKinectSensor* sensor) {
        release();
        sensor->get_CoordinateMapper(&_coordinate_mapper);
    }

    void release() {
        if (!_coordinate_mapper) {
            return;
        }
        _coordinate_mapper->Release();
        _coordinate_mapper = nullptr;
    }

    ColorSpacePoint cameraToColor(const CameraSpacePoint& camera_space_point) {
        ColorSpacePoint color_space_point;
        _coordinate_mapper->MapCameraPointToColorSpace(camera_space_point, &color_space_point);
        return std::move(color_space_point);
    }

    DepthSpacePoint cameraToDepth(const CameraSpacePoint& camera_space_point) {
        DepthSpacePoint depth_space_point;
        _coordinate_mapper->MapCameraPointToDepthSpace(camera_space_point, &depth_space_point);
        return std::move(depth_space_point);
    }

    void colorFrameToDepth(const UINT16* from_data, DepthSpacePoint* to_data) {
        _coordinate_mapper->MapColorFrameToDepthSpace(
            512*424, from_data,
            1920*1080, to_data);
    }

    void depthFrameToColor(const UINT16* from_data, ColorSpacePoint* to_data) {
        _coordinate_mapper->MapDepthFrameToColorSpace(
            512*424, from_data,
            512*424, to_data);
    }

private:
    ICoordinateMapper* _coordinate_mapper;
};

#endif
