#ifndef _COLOR_FRAME_SOURCE_WRAP_H_
#define _COLOR_FRAME_SOURCE_WRAP_H_

#include <Kinect.h>
#include <functional>
#include "FrameDescriptionWrap.h"

class ColorFrameSourceWrap {
public:
    ColorFrameSourceWrap() : _color_frame_reader(nullptr) {
    }

    ~ColorFrameSourceWrap() {
        release();
    }

    void init(IKinectSensor* sensor) {
        release();

        BOOLEAN is_open;
        sensor->get_IsOpen(&is_open);
        if (!is_open) {
            sensor->Open();
        }

        IColorFrameSource* color_frame_source;
        sensor->get_ColorFrameSource(&color_frame_source);
        color_frame_source->OpenReader(&_color_frame_reader);

        IFrameDescription* desc;
        color_frame_source->CreateFrameDescription(ColorImageFormat_Rgba, &desc);

        desc->get_Height(&_desc._height);
        desc->get_Width(&_desc._width);
        desc->get_BytesPerPixel(&_desc._bytes_per_pixel);
        desc->get_LengthInPixels(&_desc._length_in_pixels);
        desc->get_VerticalFieldOfView(&_desc._vertical_field_of_view);
        desc->get_HorizontalFieldOfView(&_desc._horizontal_field_of_view);
        desc->get_DiagonalFieldOfView(&_desc._diagonal_field_of_view);

        desc->Release();
        desc = nullptr;

        color_frame_source->Release();
        color_frame_source = nullptr;
    }

    void release() {
        if (!_color_frame_reader) {
            return;
        }
        _color_frame_reader->Release();
        _color_frame_reader = nullptr;
    }

    bool update(std::function<void(IColorFrame*)> task) const {
        if (!_color_frame_reader) {
            return false;
        }
        IColorFrame* color_frame = nullptr;
        auto ret =_color_frame_reader->AcquireLatestFrame(&color_frame);
        if (FAILED(ret)) {
            if (color_frame) {
                color_frame->Release();
                color_frame = nullptr;
            }
            return false;
        }

        task(color_frame);

        color_frame->Release();
        color_frame = nullptr;
        return true;
    }

    const FrameDescription& getFrameDescription() const {
        return _desc;
    }

private:
    IColorFrameReader* _color_frame_reader;
    FrameDescription _desc;
};

#endif
