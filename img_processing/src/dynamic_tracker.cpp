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
    const vector<ObjectInformation> &detectionOutput)
{
    id_counter = 0;
    output.clear();
    trackers.clear();
    failedCount.clear();
    this->frame = frame;
    // Create trackers for each detected object
    for (const auto &objectInformation : detectionOutput) {
        Ptr<Tracker> tracker = TrackerCSRT::create();
        tracker->init(*frame, objectInformation.position);
        trackers.push_back(tracker);
        ObjectInformation to = {id_counter++, objectInformation.type,
                                objectInformation.position,
                                objectInformation.position};
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
            output[i].prevPosition = output[i].position;
            output[i].position = bbox;
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

vector<ObjectInformation> DynamicTracker::getOutput() const
{
    return output;
}
