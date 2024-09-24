#ifndef __DETECTOR_H__
#define __DETECTOR_H__

#include <opencv2/opencv.hpp>
#include "object_information_struct.h"
#include "object_type_enum.h"
#include "log_manager.h"

class Detector {
   public:
    void init(bool isCuda);
    std::vector<ObjectInformation> getOutput() const;
    void detect(const std::shared_ptr<cv::Mat> &frame, bool isTravel);
    int getIdCounter();

   private:
    int helperForDetect;
    std::shared_ptr<cv::Mat> prevFrame;
    std::shared_ptr<cv::Mat> currentFrame;
    std::vector<ObjectInformation> output;
    cv::dnn::Net net;
    const float INPUT_WIDTH = 640.0;
    const float INPUT_HEIGHT = 640.0;
    const float SCORE_THRESHOLD = 0.2;
    const float NMS_THRESHOLD = 0.4;
    const float CONFIDENCE_THRESHOLD = 0.4;

    cv::Mat formatYolov5(const std::shared_ptr<cv::Mat> &frame);
    void loadNet(bool isCuda);
    bool isValidObjectType(int value) const;
    void detectChanges();
    void detectObjects(const std::shared_ptr<cv::Mat> &frame,
                       const cv::Point &position);
    std::vector<cv::Rect> findDifference();
    std::vector<cv::Rect> unionOverlappingRectangels(
        std::vector<cv::Rect> allChanges);
    int idCounter;
};

#endif  // __DETECTOR_H__