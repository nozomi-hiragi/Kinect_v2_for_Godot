#ifndef _DEPTH_FRAME_SOURCE_WRAP_H_
#define _DEPTH_FRAME_SOURCE_WRAP_H_

#include <Kinect.h>
#include <functional>
#include "FrameDescriptionWrap.h"

class DepthFrameSourceWrap {
public:
    DepthFrameSourceWrap() : _depth_frame_reader(nullptr) {
    }

    ~DepthFrameSourceWrap() {
        release();
    }

    void init(IKinectSensor* sensor) {
        release();

        BOOLEAN is_open;
        sensor->get_IsOpen(&is_open);
        if (!is_open) {
            sensor->Open();
        }

        IDepthFrameSource* depth_frame_source;
        sensor->get_DepthFrameSource(&depth_frame_source);
        depth_frame_source->OpenReader(&_depth_frame_reader);

        IFrameDescription* desc;
        depth_frame_source->get_FrameDescription(&desc);

        desc->get_Height(&_desc._height);
        desc->get_Width(&_desc._width);
        desc->get_BytesPerPixel(&_desc._bytes_per_pixel);
        desc->get_LengthInPixels(&_desc._length_in_pixels);
        desc->get_VerticalFieldOfView(&_desc._vertical_field_of_view);
        desc->get_HorizontalFieldOfView(&_desc._horizontal_field_of_view);
        desc->get_DiagonalFieldOfView(&_desc._diagonal_field_of_view);

        desc->Release();
        desc = nullptr;

        depth_frame_source->Release();
        depth_frame_source = nullptr;
    }

    void release() {
        if (!_depth_frame_reader) {
            return;
        }
        _depth_frame_reader->Release();
        _depth_frame_reader = nullptr;
    }

    bool update(std::function<void(IDepthFrame*)> task) const {
        if (!_depth_frame_reader) {
            return false;
        }
        IDepthFrame* depth_frame = nullptr;
        auto ret =_depth_frame_reader->AcquireLatestFrame(&depth_frame);
        if (FAILED(ret)) {
            if (depth_frame) {
                depth_frame->Release();
                depth_frame = nullptr;
            }
            return false;
        }

        task(depth_frame);

        depth_frame->Release();
        depth_frame = nullptr;
        return true;
    }

    const FrameDescription& getFrameDescription() const {
        return _desc;
    }

private:
    IDepthFrameReader* _depth_frame_reader;
    FrameDescription _desc;
};

#endif
