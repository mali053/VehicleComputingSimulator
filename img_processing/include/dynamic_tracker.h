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
    std::vector<ObjectInformation> getOutput() const;
    void tracking(const std::shared_ptr<cv::Mat> &frame);

   private:
    int id_counter = 0;
    std::shared_ptr<cv::Mat> frame;
    std::vector<ObjectInformation> output;
    std::vector<cv::Ptr<cv::Tracker>> trackers;
    std::vector<int> failedCount;
    int maxFailures = 1;
};

#endif  // __DYNAMIC_TRACKER_H__