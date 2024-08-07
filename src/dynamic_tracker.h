#ifndef __DYNAMIC_TRACKER_H__
#define __DYNAMIC_TRACKER_H__

#include <opencv2/opencv.hpp>
#include "object_type_enum.h"

struct TrackerObject {
    int id;
    ObjectType type;
    cv::Rect prevPosition;
    cv::Rect currentPosition;
};

class DynamicTracker {
   public:
    // loading the moodle
    void init();
    void track(const std::shared_ptr<cv::Mat> &prevFrame,
               const std::shared_ptr<cv::Mat> &currentFrame);
    //--maybe--:--pre and after process

   private:
    std::shared_ptr<cv::Mat> prevFrame;
    std::shared_ptr<cv::Mat> currentFrame;
    std::vector<TrackerObject> output;
};

#endif  // __DYNAMIC_TRACKER_H__
