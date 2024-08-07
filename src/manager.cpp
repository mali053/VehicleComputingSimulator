#include "manager.h"

using namespace std;
using namespace cv;

Manager::Manager(const Mat &Frame)
{
    // Constructor implementation
}

void Manager::processing(const Mat &newFrame)
{
    currentFrame = make_shared<Mat>(newFrame);
    // Use CUDA if existing
    //= argc > 1 && strcmp(argv[1], "cuda") == 0;
    bool is_cuda;
    // Loading YOLOv5 model using load_net()
    detector.init(is_cuda);
    detector.detect(currentFrame);
}

void Manager::prepareForTheNext()
{
    prevFrame = currentFrame;
}
