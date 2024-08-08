#include "dynamic_tracker.h"

using namespace std;
using namespace cv;

void DynamicTracker ::track(const shared_ptr<Mat> &prevFrame,
                            const shared_ptr<Mat> &currentFrame,
                            const vector<DetectionObject> &prevOutput,
                            const vector<DetectionObject> &currentOutput) {
  this->prevFrame = prevFrame;
  this->currentFrame = currentFrame;
}
