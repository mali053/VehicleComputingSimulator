#ifndef __DYNAMIC_TRACKER_H__
#define __DYNAMIC_TRACKER_H__

#include <opencv2/opencv.hpp>
#include "object_information_struct.h"
#include "object_type_enum.h"

class DynamicTracker {
   public:
    void init();
    void startTracking(const std::shared_ptr<cv::Mat> &frame,
                       const std::vector<ObjectInformation> &detectionOutput);
    void tracking(const std::shared_ptr<cv::Mat> &frame,
                  std::vector<ObjectInformation> &objectInformation);

   private:
    std::shared_ptr<cv::Mat> frame;
    std::vector<cv::Ptr<cv::Tracker>> trackers;
    std::vector<int> failedCount;
    int maxFailures = 1;
};

#endif  // __DYNAMIC_TRACKER_H__