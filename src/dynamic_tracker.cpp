#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/opencv.hpp>
#include "dynamic_tracker.h"

using namespace std;
using namespace cv;

void DynamicTracker::init() {}
void DynamicTracker::track(const std::shared_ptr<cv::Mat> &prevFrame,
                           const std::shared_ptr<cv::Mat> &currentFrame,
                           const std::vector<DetectionObject> &prevOutput,
                           const std::vector<DetectionObject> &currentOutput)
{
    output.clear();
    this->prevFrame = prevFrame;
    this->currentFrame = currentFrame;
    // Create trackers for each detected object
    vector<Ptr<Tracker>> trackers;
    for (const auto &detectionObj : prevOutput) {
        Ptr<Tracker> tracker = TrackerCSRT::create();
        tracker->init(*prevFrame, detectionObj.position);
        trackers.push_back(tracker);
    }
    Rect bbox;
    // Update tracking results for each tracker
    for (size_t i = 0; i < trackers.size(); ++i) {
        bool ok = trackers[i]->update(*currentFrame, bbox);
        if (ok) {
            // Successful tracking
            TrackerObject trackerObject;
            trackerObject.prevPosition = (prevOutput)[i].position;
            trackerObject.id = (prevOutput)[i].id;
            trackerObject.currentPosition = bbox;
            trackerObject.type = (prevOutput)[i].type;
            this->output.push_back(trackerObject);
        }
        else {
            cout << "Tracking failed for object ID: " << (prevOutput)[i].id
                 << endl;
        }
    }
}

vector<TrackerObject> DynamicTracker::getOutput() const
{
    return output;
}
void DynamicTracker ::track(const shared_ptr<Mat> &prevFrame,
                            const shared_ptr<Mat> &currentFrame,
                            const vector<DetectionObject> &prevOutput,
                            const vector<DetectionObject> &currentOutput) {
  this->prevFrame = prevFrame;
  this->currentFrame = currentFrame;
}
