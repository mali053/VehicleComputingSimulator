#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/opencv.hpp>
#include "dynamic_tracker.h"

using namespace std;
using namespace cv;
using namespace chrono;

void DynamicTracker::init() {}

void DynamicTracker::startTracking(
    const shared_ptr<Mat> &frame,
    const vector<DetectionObject> &detectionOutput)
{
    id_counter=0;
    output.clear();
    trackers.clear();
    failedCount.clear();
    this->frame = frame;
    // Create trackers for each detected object
    for (const auto &detectionObj : detectionOutput) {
        Ptr<Tracker> tracker = TrackerCSRT::create();
        tracker->init(*frame, detectionObj.position);
        trackers.push_back(tracker);
        TrackerObject to = {id_counter++, detectionObj.type,
                            detectionObj.position, detectionObj.position};
        output.push_back(to);
        failedCount.push_back(0);
    }
}

void DynamicTracker::tracking(const shared_ptr<Mat> &frame)
{
    this->frame = frame;
    Rect bbox;
    // Update tracking results for each tracker
    for (size_t i = 0; i < trackers.size(); ++i) {
        bool ok = trackers[i]->update(*frame, bbox);
        if (ok) {
            output[i].prevPosition = output[i].currentPosition;
            output[i].currentPosition = bbox;
            failedCount[i] = 0;  // Reset failure count on successful tracking
        }
        else {
            failedCount[i]++;
            if (failedCount[i] > maxFailures) {
                trackers.erase(trackers.begin() + i);
                output.erase(output.begin() + i);
                failedCount.erase(failedCount.begin() + i);
                --i;  // Adjust index after erasing
            }
        }
    }
}

vector<TrackerObject> DynamicTracker::getOutput() const
{
    return output;
}
