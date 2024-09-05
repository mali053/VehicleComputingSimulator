#ifndef __DETECTOR_H__
#define __DETECTOR_H__

#include <opencv2/opencv.hpp>
#include "detection_object_struct.h"
#include "object_type_enum.h"

class Detector {
   public:
    // loading the moodle
    void init(bool isCuda);
    std::vector<DetectionObject> getOutput() const;
    // detect cars and peoples in the frame
    void detect(const std::shared_ptr<cv::Mat> &frame);
    //--maybe--:--pre and after process

   private:
    int helperForDetect;
    std::shared_ptr<cv::Mat> prevFrame;
    std::shared_ptr<cv::Mat> currentFrame;
    std::vector<DetectionObject> output;
    cv::dnn::Net net;
    const float INPUT_WIDTH = 640.0;
    const float INPUT_HEIGHT = 640.0;
    const float SCORE_THRESHOLD = 0.2;
    // This threshold used for non-maximum suppression
    // to remove overlapping bounding boxes
    const float NMS_THRESHOLD = 0.4;
    const float CONFIDENCE_THRESHOLD = 0.4;
    
    cv::Mat formatYolov5(const std::shared_ptr<cv::Mat>& frame);
    void loadNet(bool isCuda);
    bool isValidObjectType(int value) const;
    void detectObjects(const std::shared_ptr<cv::Mat>& frame, const cv::Point& position);
    void detectChanges();
    std::vector<cv::Rect> findDifference();
    std::vector<cv::Rect> unionOverlappingRectangels(
        std::vector<cv::Rect> allChanges);
};

#endif // __DETECTOR_H__