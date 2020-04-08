#ifndef _KINECT_SENSOR_WRAP_H_
#define _KINECT_SENSOR_WRAP_H_

#include <Kinect.h>

class KinectSensorWrap {
public:
    KinectSensorWrap() : _kinect_sensor(nullptr) {}
    ~KinectSensorWrap() {}

    void init() {
        if (FAILED(GetDefaultKinectSensor(&_kinect_sensor))) { return; }
        BOOLEAN is_open = false;
        _kinect_sensor->get_IsOpen(&is_open);
        if (!is_open) {
            _kinect_sensor->Open();
        }
    }

    void release() {
        if (!_kinect_sensor) {
            return;
        }
        BOOLEAN is_open = false;
        _kinect_sensor->get_IsOpen(&is_open);
        if (is_open) {
            _kinect_sensor->Close();
        }
        _kinect_sensor->Release();
        _kinect_sensor = nullptr;
    }

    template<class T> std::unique_ptr<T> create() const {
        auto instance = std::make_unique<T>();
        instance->init(_kinect_sensor);
        return std::move(instance);
    }

private:
    IKinectSensor* _kinect_sensor;
};

#endif
