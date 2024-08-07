#ifndef __DETECTOR_H__
#define __DETECTOR_H__

#include <opencv2/opencv.hpp>
#include "object_type_enum.h"

struct DetectionObject {
    int id;
    ObjectType type;
    float confidence;
    // Mat imageView;
    cv::Rect position;
};

class Detector {
   public:
    // loading the moodle
    void init(bool isCuda);
    std::vector<DetectionObject> getOutput() const;
    // detect cars and peoples in the frame
    void detect(const std::shared_ptr<cv::Mat> &frame);
    //--maybe--:--pre and after process

   private:
    std::shared_ptr<cv::Mat> frame;
    std::vector<DetectionObject> output;
    cv::dnn::Net net;
    const float INPUT_WIDTH = 640.0;
    const float INPUT_HEIGHT = 640.0;
    const float SCORE_THRESHOLD = 0.2;
    // This threshold used for non-maximum suppression
    // to remove overlapping bounding boxes
    const float NMS_THRESHOLD = 0.4;
    const float CONFIDENCE_THRESHOLD = 0.4;

    cv::Mat formatYolov5();
    void loadNet(bool isCuda);
    bool isValidObjectType(int value) const;
};

#endif  // __DETECTOR_H__