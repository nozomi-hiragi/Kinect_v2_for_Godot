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

    auto kari(){return _coordinate_mapper;}

private:
    ICoordinateMapper* _coordinate_mapper;
};

#endif
