#include "manager.h"

using namespace std;
using namespace cv;

Manager::Manager(const Mat &Frame)
{
    // Constructor implementation
}

void Manager::init()
{
    bool isCuda = false;
    detector.init(isCuda);
    dynamicTracker.init();
}

void Manager::processing(const Mat &newFrame, bool isTravel)
{
    currentFrame = make_shared<Mat>(newFrame);
    // Use CUDA if existing
    //= argc > 1 && strcmp(argv[1], "cuda") == 0;
    bool is_cuda;
    // Loading YOLOv5 model using load_net()
    detector.init(is_cuda);
    // TODO :call to findDifference - A function to find the difference between
    // two frames This function will happen to detect for any change it find
    detector.detect(this->currentFrame, isTravel);
    alerter.sendAlerts(this->detector.getOutput());
}

void Manager::prepareForTheNext()
{
    prevFrame = currentFrame;
}
