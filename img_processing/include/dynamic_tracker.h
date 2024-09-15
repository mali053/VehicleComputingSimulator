#ifndef __DYNAMIC_TRACKER_H__
#define __DYNAMIC_TRACKER_H__

#include <opencv2/opencv.hpp>
#include "detection_object_struct.h"
#include "object_type_enum.h"
#include "tracker_object_struct.h"
class DynamicTracker {
   public:
    void init();
    void startTracking(const std::shared_ptr<cv::Mat> &frame,
                       const std::vector<DetectionObject> &detectionOutput);
    std::vector<TrackerObject> getOutput() const;
    void tracking(const std::shared_ptr<cv::Mat> &frame);

   private:
    int id_counter = 0;
    std::shared_ptr<cv::Mat> frame;
    std::vector<TrackerObject> output;
    std::vector<cv::Ptr<cv::Tracker>> trackers;
    std::vector<int> failedCount;
    int maxFailures = 1;
};

#endif  // __DYNAMIC_TRACKER_H__