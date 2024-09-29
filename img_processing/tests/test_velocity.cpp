#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <gtest/gtest.h>
#include "velocity.h"
#include "dynamic_tracker.h"
#include "detector.h"
#include "distance.h"

using namespace std;
using namespace cv;

TEST(TVelocity, calculate_TVelocity)
{
    // Load a real image from file
    string imagePath = "../tests/images/black_line.JPG";
    Mat calibrationImage;
    calibrationImage = imread(imagePath);
    if (calibrationImage.empty()) {
        throw runtime_error("Could not open or find the image");
    }

    Distance &distance = Distance::getInstance(calibrationImage);

    Detector detector;
    DynamicTracker tracker;
    Velocity velocity;
    velocity.init(0.04);
    detector.init(false);
    tracker.init();
    VideoCapture capture("../tests/images/one_car.mp4");
    Mat frame;
    capture.read(frame);
    int cnt = 0;
    while (!frame.empty()) {
        shared_ptr<Mat> f1 = make_shared<Mat>(frame);
        auto detectionOutput = make_shared<vector<ObjectInformation>>();
        auto trackingOutput = make_shared<vector<ObjectInformation>>();
        detector.detect(f1, true);
        *detectionOutput = detector.getOutput();
        *trackingOutput = detector.getOutput();
        tracker.startTracking(f1, *detectionOutput);

        for (int i = 0; i < 10; i++) {
            capture.read(frame);
            cnt++;
            if (frame.empty())
                return;
            shared_ptr<Mat> frame1 = make_shared<Mat>(frame);
            tracker.tracking(frame1, *trackingOutput);
            distance.findDistance(*trackingOutput);
            velocity.returnVelocities(*trackingOutput);

            for (int i = 0; i < (*trackingOutput).size(); i++) {
                rectangle(*frame1, (*trackingOutput)[i].position,
                          Scalar(256, 0, 0), 2);

                Point textPosition((*trackingOutput)[i].position.x,
                                   (*trackingOutput)[i].position.y - 10);

                // putText(*frame1, "Speed: " + std::to_string((*trackingOutput)[i].speed), textPosition,
                /// FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 2);
                putText(
                    *frame1,
                    "Speed: " + std::to_string((*trackingOutput)[i].distance),
                    textPosition, FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0),
                    2);
            }
            imshow("frame", *frame1);
            waitKey(0);
            cout << cnt << endl;
        }
        capture.read(frame);
        cnt++;
    }
}