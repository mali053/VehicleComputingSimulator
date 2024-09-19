#ifndef __OBJECT_INFORMATION_STRUCT_H__
#define __OBJECT_INFORMATION_STRUCT_H__

#include <opencv2/opencv.hpp>
#include "object_type_enum.h"

struct ObjectInformation {
    int id;
    ObjectType type;
    cv::Rect prevPosition;
    cv::Rect position;
    double distance;
};

#endif  // __OBJECT_INFORMATION_STRUCT_H__