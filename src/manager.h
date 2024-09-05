#ifndef __MANAGER_H__
#define __MANAGER_H__

#include <opencv2/opencv.hpp>
#include "alerter.h"
#include "detector.h"
#include "dynamic_tracker.h"

class Manager {
   public:
    // Gets a first image and initializes the class's members
    Manager(const cv::Mat& Frame);
    // Gets the currentFrame and sends it for detection and then tracking,
    // finally if necessary sends a alert
    void processing(const cv::Mat& newFrame);
    void findDifference();
    void init();

   private:
    std::shared_ptr<cv::Mat> prevFrame;
    std::shared_ptr<cv::Mat> currentFrame;
    std::vector<DetectionObject> prevOutput;
    std::vector<DetectionObject> currentOutput;
    Detector detector;
    DynamicTracker dynamicTracker;
    Alerter alerter;
    
    // Moves the current image to the prevFrame
    // and clears the memory of the currentFrame;
    void prepareForTheNext();
};

#endif //__MANAGER_H__