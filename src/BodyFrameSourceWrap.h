#ifndef _BODY_FRAME_SOURCE_WRAP_H_
#define _BODY_FRAME_SOURCE_WRAP_H_

#include <Kinect.h>
#include <functional>
#include "FrameDescriptionWrap.h"

class BodyFrameSourceWrap {
public:
    BodyFrameSourceWrap() : _body_frame_reader(nullptr) {
    }

    ~BodyFrameSourceWrap() {
        release();
    }

    void init(IKinectSensor* sensor) {
        release();

        BOOLEAN is_open;
        sensor->get_IsOpen(&is_open);
        if (!is_open) {
            sensor->Open();
        }

        IBodyFrameSource* body_frame_source;
        sensor->get_BodyFrameSource(&body_frame_source);
        body_frame_source->OpenReader(&_body_frame_reader);

        body_frame_source->Release();
        body_frame_source = nullptr;
    }

    void release() {
        if (!_body_frame_reader) {
            return;
        }
        _body_frame_reader->Release();
        _body_frame_reader = nullptr;
    }

    bool update(std::function<void(IBodyFrame*)> task) const {
        if (!_body_frame_reader) {
            return false;
        }
        IBodyFrame* body_frame = nullptr;
        auto ret =_body_frame_reader->AcquireLatestFrame(&body_frame);
        if (FAILED(ret)) {
            if (body_frame) {
                body_frame->Release();
                body_frame = nullptr;
            }
            return false;
        }

        task(body_frame);

        body_frame->Release();
        body_frame = nullptr;
        return true;
    }

    auto kari(){return _body_frame_reader;}

private:
    IBodyFrameReader* _body_frame_reader;
};

#endif
