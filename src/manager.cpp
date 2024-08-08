#include "manager.h"

using namespace std;
using namespace cv;

Manager::Manager(const Mat &Frame) {
  // Constructor implementation
}

void Manager::init() {
  bool isCuda = false;
  detector.init(isCuda);
  dynamicTracker.init();
}

void Manager::processing(const Mat &newFrame) {
  currentFrame = make_shared<Mat>(newFrame);
  // Use CUDA if existing
  //= argc > 1 && strcmp(argv[1], "cuda") == 0;
  bool is_cuda; 
  // Loading YOLOv5 model using load_net()
  detector.init(is_cuda);
  // TODO :call to findDifference - A function to find the difference between
  // two frames This function will happen to detect for any change it find
  detector.detect(currentFrame);
  alerter.sendAlerts(this->detector.getOutput());
}

void Manager::prepareForTheNext() { prevFrame = currentFrame; }

void Manager::findDifference() {
  Mat gray1, gray2;
  cvtColor(*prevFrame, gray1, COLOR_BGR2GRAY);
  cvtColor(*currentFrame, gray2, COLOR_BGR2GRAY);
  // Find the difference between the two images
  Mat diff;
  absdiff(gray1, gray2, diff);
  // Apply threshold
  Mat thresh;
  threshold(diff, thresh, 0, 255, THRESH_BINARY | THRESH_OTSU);
  // Dilation
  Mat kernel = Mat::ones(5, 5, CV_8U);
  Mat dil;
  dilate(thresh, dil, kernel, Point(-1, -1), 2);
  // Calculate contours
  vector<vector<Point>> contours;
  findContours(dil, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
  for (const auto &contour : contours) {
    // Calculate bounding box around contour
    Rect boundingBox = boundingRect(contour);
    // TODO : Sending each rectangle to detect
  }
}