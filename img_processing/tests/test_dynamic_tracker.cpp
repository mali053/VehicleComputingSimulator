#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <gtest/gtest.h>
#include <string>
#include "dynamic_tracker.h"
#include "manager.h"
#include "detector.h"
#include "utils.h"
#include "log_manager.h"

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
    Detector detector;
    DynamicTracker tracker;
    detector.init(false);
    tracker.init();
    Mat img1 = imread("../tests/images/track_2_cars_first_frame.jpg");
    Mat img2 = imread("../tests/images/track_2_cars_second_frame.jpg");
    if (img1.empty() || img2.empty()) {
        LogManager::logErrorMessage(ErrorType::IMAGE_ERROR,
                                    "Could not load images");
        return;
    }
    shared_ptr<Mat> prevFrame = make_shared<Mat>(img1);
    shared_ptr<Mat> currentFrame = make_shared<Mat>(img2);
    auto prevOutput = vector<ObjectInformation>();
    auto detectionOutput = vector<ObjectInformation>();
    auto trackingOutput = vector<ObjectInformation>();
    detector.detect(prevFrame, true);
    prevOutput = detector.getOutput();
    detector.detect(currentFrame, true);
    detectionOutput = detector.getOutput();
    trackingOutput = detector.getOutput();

    // check time - start
    auto start = std::chrono::high_resolution_clock::now();
    tracker.startTracking(prevFrame, trackingOutput);
    tracker.tracking(currentFrame, trackingOutput);
    // check time - end
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    LogManager::logInfoMessage(InfoType::EXECUTION_TIME,
                               to_string(elapsed.count()) + " ms");
    int i = 0;
    float result;

    for (const auto &tracktion : trackingOutput) {
        LogManager::logInfoMessage(
            InfoType::TRACKING,
            "ID: " + to_string(tracktion.id) +
                " Type: " + to_string(tracktion.type) +
                " Position: " + rectToString(tracktion.position));
        result = calculateIoU(tracktion.position, detectionOutput[i].position);
        LogManager::logInfoMessage(InfoType::IOU,
                                   "calculate- " + to_string(result));
        i++;
    }
}

TEST(Track, track_video)
{
    Detector detector;
    DynamicTracker tracker;
    detector.init(false);
    tracker.init();
    VideoCapture capture("../tests/images/one_car.mp4");
    Mat frame;
    capture.read(frame);

    while (!frame.empty()) {
        shared_ptr<Mat> f1 = make_shared<Mat>(frame);
        auto output = vector<ObjectInformation>();
        detector.detect(f1, true);
        output = detector.getOutput();
        vector<Scalar> colours = {
            Scalar(255, 0, 0),     // Blue
            Scalar(0, 255, 0),     // Green
            Scalar(0, 0, 255),     // Red
            Scalar(255, 255, 0),   // Cyan
            Scalar(255, 0, 255),   // Magenta
            Scalar(0, 255, 255),   // Yellow
            Scalar(128, 0, 128),   // Purple
            Scalar(0, 128, 128),   // Teal
            Scalar(128, 128, 0),   // Olive
            Scalar(128, 128, 128)  // Gray
        };
        tracker.startTracking(f1, output);
        for (int i = 0; i < (output).size(); i++) {
            colours.push_back(Scalar(i * 40, i * 20, i * 50));
            rectangle(frame, (output)[i].position, colours[i]);
        }
        cv::imshow("output", frame);
        // cv::waitKey(0);
        for (int i = 0; i < 10; i++) {
            capture.read(frame);
            if (frame.empty()) {
                LogManager::logInfoMessage(InfoType::MEDIA_FINISH);
                return;
            }
            // auto start = std::chrono::high_resolution_clock::now();
            shared_ptr<Mat> frame1 = make_shared<Mat>(frame);
            tracker.tracking(frame1, output);
            for (int j = 0; j < (output).size(); j++) {
                rectangle(frame, (output)[j].position, colours[j]);
            }
            cv::imshow("output", frame);
            // cv::waitKey(0);
        }
        capture.read(frame);
    }
}

TEST(Track, calculate_execution_time)
{
    Detector detector;
    DynamicTracker tracker;
    detector.init(false);
    tracker.init();
    VideoCapture capture("../tests/images/one_car.mp4");
    Mat frame;
    capture.read(frame);
    // check time - start
    auto start = std::chrono::high_resolution_clock::now();

    for (int z = 0; z < 5; z++) {
        shared_ptr<Mat> f1 = make_shared<Mat>(frame);
        auto output = vector<ObjectInformation>();
        detector.detect(f1, true);
        output = detector.getOutput();
        tracker.startTracking(f1, output);

        for (int i = 0; i < 10; i++) {
            capture.read(frame);
            if (frame.empty()) {
                LogManager::logInfoMessage(InfoType::MEDIA_FINISH);
                return;
            }
            // auto start = std::chrono::high_resolution_clock::now();
            shared_ptr<Mat> frame1 = make_shared<Mat>(frame);
            tracker.tracking(frame1, output);
        }
        capture.read(frame);
    }
    // check time - end
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    LogManager::logInfoMessage(
        InfoType::EXECUTION_TIME,
        "tracking- " + to_string(elapsed.count()) + " ms");
    VideoCapture capture1("../tests/images/one_car.mp4");
    frame;
    capture1.read(frame);

    //check time - start
    start = std::chrono::high_resolution_clock::now();
    for (int z = 0; z < 50; z++) {
        std::shared_ptr<cv::Mat> f = std::make_shared<cv::Mat>(frame);
        detector.detect(f, true);
        capture1.read(frame);
    }
    // check time - end
    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;
    LogManager::logInfoMessage(
        InfoType::EXECUTION_TIME,
        "tracking- " + to_string(elapsed.count()) + " ms");
}

TEST(Track, calculate_iou)
{
    Detector detector;
    DynamicTracker tracker;
    detector.init(false);
    tracker.init();
    VideoCapture capture("../tests/images/close_cars.mov");
    Mat frame;
    capture.read(frame);

    for (int z = 0; z < 5; z++) {
        shared_ptr<Mat> f1 = make_shared<Mat>(frame);
        auto detectionOutput = vector<ObjectInformation>();
        auto trackingOutput = vector<ObjectInformation>();
        detector.detect(f1, true);
        detectionOutput = detector.getOutput();
        trackingOutput = detector.getOutput();
        tracker.startTracking(f1, detectionOutput);

        for (int i = 0; i < 50; i++) {
            capture.read(frame);
            if (frame.empty()) {
                LogManager::logInfoMessage(InfoType::MEDIA_FINISH);
                return;
            }
            shared_ptr<Mat> frame1 = make_shared<Mat>(frame);
            tracker.tracking(frame1, trackingOutput);
            detector.detect(f1, true);
            detectionOutput = detector.getOutput();
            rectangle(*frame1, trackingOutput[0].position, Scalar(256, 0, 0),
                      3);
            rectangle(*frame1, detectionOutput[0].position, Scalar(0, 0, 256),
                      3);
            imshow("frame1", *frame1);
            //waitKey(0);
            float iou = calculateIoU((detectionOutput)[0].position,
                                     (trackingOutput)[0].position);
            LogManager::logDebugMessage(DebugType::PRINT,
                                        "iou " + to_string(iou));
        }
        capture.read(frame);
    }
}

TEST(Track, track_with_few_detection)
{
    Detector detector;
    DynamicTracker tracker;
    detector.init(false);
    tracker.init();
    VideoCapture capture("../tests/images/video13.mp4");
    Mat frame;
    capture.read(frame);
    shared_ptr<Mat> f1 = make_shared<Mat>(frame);
    auto detectionOutput = vector<ObjectInformation>();
    auto trackingOutput = vector<ObjectInformation>();
    detector.detect(f1, true);
    detectionOutput = detector.getOutput();
    trackingOutput = detector.getOutput();
    tracker.startTracking(f1, detectionOutput);
    int cnt = 0;

    while (!frame.empty()) {
        capture.read(frame);
        shared_ptr<Mat> frame1 = make_shared<Mat>(frame);
        tracker.tracking(frame1, trackingOutput);
        detector.detect(f1, true);
        detectionOutput = detector.getOutput();

        for (int i = 0; i < trackingOutput.size(); i++) {
            Rect pos = trackingOutput[i].position;
            if (0 <= pos.x && 0 <= pos.y && pos.x + pos.width <= frame1->cols &&
                pos.y + pos.height <= frame1->rows) {
                rectangle(*frame1, pos, Scalar(256, 0, 0), 2);
            }
        }

        for (int i = 0; i < detectionOutput.size(); i++) {
            Rect pos = detectionOutput[i].position;
            if (0 <= pos.x && 0 <= pos.y && pos.x + pos.width <= frame1->cols &&
                pos.y + pos.height <= frame1->rows) {
                rectangle(*frame1, pos, Scalar(256, 0, 0), 2);
            }
        }
        imshow("frame1", *frame1);
        LogManager::logDebugMessage(DebugType::PRINT,
                                    "cnt++ " + to_string(cnt));
        //waitKey(1);
    }
}

TEST(Track, calculate_detection_per_frames)
{
    Detector detector;
    DynamicTracker tracker;
    detector.init(false);
    tracker.init();
    VideoCapture capture("../tests/images/video13.mp4");
    Mat frame;
    capture.read(frame);
    int cnt = 0;

    while (!frame.empty()) {
        shared_ptr<Mat> f1 = make_shared<Mat>(frame);
        auto detectionOutput = vector<ObjectInformation>();
        auto trackingOutput = vector<ObjectInformation>();
        detector.detect(f1, true);
        detectionOutput = detector.getOutput();
        trackingOutput = detector.getOutput();
        tracker.startTracking(f1, detectionOutput);

        for (int i = 0; i < 20; i++) {
            capture.read(frame);
            cnt++;
            if (frame.empty()) {
                LogManager::logInfoMessage(InfoType::MEDIA_FINISH);
                return;
            }
            shared_ptr<Mat> frame1 = make_shared<Mat>(frame);
            tracker.tracking(frame1, trackingOutput);
            detector.detect(f1, true);
            detectionOutput = detector.getOutput();

            for (int i = 0; i < trackingOutput.size(); i++) {
                rectangle(*frame1, trackingOutput[i].position,
                          Scalar(256, 0, 0), 2);
            }
            for (int i = 0; i < detectionOutput.size(); i++) {
                rectangle(*frame1, detectionOutput[i].position,
                          Scalar(0, 0, 256), 2);
            }
            imshow("frame", *frame1);
            //waitKey(1);
            LogManager::logDebugMessage(DebugType::PRINT,
                                        "cnt++ " + to_string(cnt));
        }
        capture.read(frame);
        cnt++;
    }
}
