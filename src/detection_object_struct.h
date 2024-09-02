#ifndef __DETECTION_OBJECT_STRUCT_H__
#define __DETECTION_OBJECT_STRUCT_H__

#include <opencv2/opencv.hpp>
#include "object_type_enum.h"

struct DetectionObject {
  int id;
  ObjectType type;
  float confidence;
  cv::Rect position;
};

#endif  //__DETECTION_OBJECT_STRUCT_H__