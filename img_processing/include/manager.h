#ifndef __MANAGER_H__
#define __MANAGER_H__

#include <opencv2/opencv.hpp>
#include "alerter.h"
#include "detector.h"
#include "distance.h"
#include "dynamic_tracker.h"
#include "log_manager.h"
#include "velocity.h"
#include "communication.h"
class Manager {
   public:
    static logger imgLogger;
    Manager(int processID);
    // Gets the currentFrame and sends it for detection and then tracking,
    // finally if necessary sends a alert
    int processing(const cv::Mat &newFrame, bool mode);
    void mainDemo();
    // init all variabels and creat the instance of distance
    void init();

   private:
    Communication communication;
    std::shared_ptr<cv::Mat> prevFrame;
    std::shared_ptr<cv::Mat> currentFrame;
    std::vector<ObjectInformation> prevOutput;
    std::vector<ObjectInformation> currentOutput;
    Detector detector;
    Velocity velocity;
    DynamicTracker dynamicTracker;
    Alerter alerter;
    int iterationCnt;
    uint32_t destID;
    uint32_t processID;

    // Moves the current image to the prevFrame
    // and clears the memory of the currentFrame;
    void prepareForTheNext();
    void drawOutput();
    bool isDetect(bool isTravel);
    bool isResetTracker(bool isTravel);
    bool isTrack(bool isTravel);
    void sendAlerts(std::vector<std::vector<uint8_t>> &alerts);
    void runOnVideo(std::string videoPath);
    int readIdFromJson(const char *target);
};
#endif  //__MANAGER_H__
