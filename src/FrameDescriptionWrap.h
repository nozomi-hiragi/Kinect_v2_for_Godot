#ifndef _FRAME_DESCRIPTION_WRAP_H_
#define _FRAME_DESCRIPTION_WRAP_H_

#include <Kinect.h>

struct FrameDescription {
    FrameDescription()
    : _width(0)
    , _height(0)
    , _bytes_per_pixel(0)
    , _length_in_pixels(0)
    , _vertical_field_of_view(0)
    , _horizontal_field_of_view(0)
    , _diagonal_field_of_view(0)
    {}

    int _width;
    int _height;
    UINT _bytes_per_pixel;
    UINT _length_in_pixels;
    float _vertical_field_of_view;
    float _horizontal_field_of_view;
    float _diagonal_field_of_view;
};

#endif
