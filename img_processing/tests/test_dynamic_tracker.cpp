#include <string>
#include <vector>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <gtest/gtest.h>
#include "dynamic_tracker.h"
#include "detector.h"

using namespace std;
using namespace cv;

float calculateIoU(const Rect &rect1, const Rect &rect2)
{
    // Calculate the coordinates of the bottom-right corners of the rectangles
    int x1_right = rect1.x + rect1.width;
    int y1_bottom = rect1.y + rect1.height;
    int x2_right = rect2.x + rect2.width;
    int y2_bottom = rect2.y + rect2.height;

    // Calculate the coordinates of the intersection
    int ix_left = std::max(rect1.x, rect2.x);
    int iy_top = std::max(rect1.y, rect2.y);
    int ix_right = std::min(x1_right, x2_right);
    int iy_bottom = std::min(y1_bottom, y2_bottom);

    // Calculate the area of the intersection
    int iw = std::max(0, ix_right - ix_left);
    int ih = std::max(0, iy_bottom - iy_top);
    float intersectionArea = static_cast<float>(iw * ih);

    // Calculate the area of each rectangle
    float rect1Area = static_cast<float>(rect1.width * rect1.height);
    float rect2Area = static_cast<float>(rect2.width * rect2.height);

    // Calculate the area of the union
    float unionArea = rect1Area + rect2Area - intersectionArea;

    // Calculate IoU
    float iou = (unionArea != 0) ? (intersectionArea / unionArea) : 0;

    return iou;
}

TEST(Track, twoCars)
{
    cout << "TEST Twocars" << endl;
    Detector detector;
    DynamicTracker tracker;
    detector.init(false);
    tracker.init();
    Mat img1 = imread("../tests/images/track_2_cars_first_frame.jpg");
    Mat img2 = imread("../tests/images/track_2_cars_second_frame.jpg");
    if (img1.empty() || img2.empty()) {
        cerr << "Error: Could not load images!" << endl;
    }
    shared_ptr<Mat> prevFrame = make_shared<Mat>(img1);
    shared_ptr<Mat> currentFrame = make_shared<Mat>(img2);
    auto prevOutput = make_shared<vector<DetectionObject>>();
    auto currentOutput = make_shared<vector<DetectionObject>>();
    detector.detect(prevFrame, true);
    *prevOutput = detector.getOutput();
    detector.detect(currentFrame, true);
    *currentOutput = detector.getOutput();
    //check time - start
    auto start = std::chrono::high_resolution_clock::now();
    tracker.startTracking(prevFrame, *prevOutput);
    tracker.tracking(currentFrame);
    //check time - end
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Execution time: " << elapsed.count() << " ms" << std::endl;
    int i = 0;
    float result;
    for (const auto &tracktion : tracker.getOutput()) {
        std::cout << " ID: " << tracktion.id << ", Type: " << tracktion.type
                  << ", position: " << tracktion.currentPosition << std::endl;
        result = calculateIoU(tracktion.currentPosition, (*currentOutput)[i].position);

        cout << "calculateIoU " << result << endl;
        i++;
    }
}
