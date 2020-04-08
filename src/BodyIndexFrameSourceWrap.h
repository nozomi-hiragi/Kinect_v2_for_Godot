#ifndef _BODY_INDEX_FRAME_SOURCE_WRAP_H_
#define _BODY_INDEX_FRAME_SOURCE_WRAP_H_

#include <Kinect.h>
#include <functional>
#include "FrameDescriptionWrap.h"

class BodyIndexFrameSourceWrap {
public:
    BodyIndexFrameSourceWrap() : _body_index_frame_reader(nullptr) {
    }

    ~BodyIndexFrameSourceWrap() {
        release();
    }

    void init(IKinectSensor* sensor) {
        release();

        BOOLEAN is_open;
        sensor->get_IsOpen(&is_open);
        if (!is_open) {
            sensor->Open();
        }

        IBodyIndexFrameSource* body_index_frame_source;
        sensor->get_BodyIndexFrameSource(&body_index_frame_source);
        body_index_frame_source->OpenReader(&_body_index_frame_reader);

        IFrameDescription* desc;
        body_index_frame_source->get_FrameDescription(&desc);

        desc->get_Height(&_desc._height);
        desc->get_Width(&_desc._width);
        desc->get_BytesPerPixel(&_desc._bytes_per_pixel);
        desc->get_LengthInPixels(&_desc._length_in_pixels);
        desc->get_VerticalFieldOfView(&_desc._vertical_field_of_view);
        desc->get_HorizontalFieldOfView(&_desc._horizontal_field_of_view);
        desc->get_DiagonalFieldOfView(&_desc._diagonal_field_of_view);

        desc->Release();
        desc = nullptr;

        body_index_frame_source->Release();
        body_index_frame_source = nullptr;
    }

    void release() {
        if (!_body_index_frame_reader) {
            return;
        }
        _body_index_frame_reader->Release();
        _body_index_frame_reader = nullptr;
    }

    bool update(std::function<void(IBodyIndexFrame*)> task) const {
        if (!_body_index_frame_reader) {
            return false;
        }
        IBodyIndexFrame* body_index_frame = nullptr;
        auto ret =_body_index_frame_reader->AcquireLatestFrame(&body_index_frame);
        if (FAILED(ret)) {
            if (body_index_frame) {
                body_index_frame->Release();
                body_index_frame = nullptr;
            }
            return false;
        }

        task(body_index_frame);

        body_index_frame->Release();
        body_index_frame = nullptr;
        return true;
    }

    const FrameDescription& getFrameDescription() const {
        return _desc;
    }

    auto kari(){return _body_index_frame_reader;}

private:
    IBodyIndexFrameReader* _body_index_frame_reader;
    FrameDescription _desc;
};

#endif
