#ifndef _COORDINATOR_MAPPER_WRAP_H_
#define _COORDINATOR_MAPPER_WRAP_H_

#include <Kinect.h>

class CoordinatorMapperWrap {
public:
    CoordinatorMapperWrap() : _coordinate_mapper(nullptr) {}
    ~CoordinatorMapperWrap() {}

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
