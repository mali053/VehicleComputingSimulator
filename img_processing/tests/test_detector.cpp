#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>
#include <string>
#include "detector.h"
#include "manager.h"
#include "utils.h"
#include "log_manager.h"

using namespace std;
using namespace cv;

// or true
bool is_cuda = false;

// Helper function to load an image from a file
Mat loadImage(const string &filename)
{
    Mat img = imread(filename);
    if (img.empty()) {
        LogManager::logErrorMessage(ErrorType::IMAGE_ERROR,
                                    "Could not load image");
        throw runtime_error("Could not open or find the image");
    }
    return img;
}

// Test case for Detector::detect with a real image containing 2 cars
TEST(DetectorTest, DetectTwoCars)
{
    // Load a real image from file
    string imagePath = "../tests/images/two_cars_image.png";
    Mat testImage;
    try {
        testImage = loadImage(imagePath);
    }
    catch (const runtime_error &e) {
        LogManager::logErrorMessage(ErrorType::IMAGE_ERROR, string(e.what()));
        FAIL() << e.what();
    }

    // Wrap it in a shared_ptr
    shared_ptr<Mat> frame = make_shared<Mat>(testImage);

    // Create Detector instance
    Detector detector;

    // bool is_cuda = false; // or true, depending on your setup
    detector.init(is_cuda);

    // Perform detection
    detector.detect(frame, true);

    // Get output
    const auto &output = detector.getOutput();

    // Check if output is not empty
    ASSERT_FALSE(output.empty());

    // Print results for manual inspection
    int peopleCount = 0, carCount = 0;
    for (const auto &detection : output) {
        LogManager::logInfoMessage(
            InfoType::DETECTION,
            "ID: " + to_string(detection.id) +
                " Type: " + to_string(detection.type) +
                " Position: " + rectToString(detection.position));
        // Assuming ObjectType::CAR corresponds to the type of car in the enum
        if (detection.type == ObjectType::CAR) {
            carCount++;
        }
        else
            peopleCount++;
    }

    // Assert that exactly 2 cars are detected
    ASSERT_EQ(carCount, 2);
    ASSERT_EQ(peopleCount, 0);
}

// Test case for Detector::detect with a real image containing 3 cars
TEST(DetectorTest, DetectThreeCars)
{
    // Load a real image from file
    string imagePath = "../tests/images/three_cars_image.png";
    Mat testImage;
    try {
        testImage = loadImage(imagePath);
    }
    catch (const runtime_error &e) {
        LogManager::logErrorMessage(ErrorType::IMAGE_ERROR, string(e.what()));
        FAIL() << e.what();
    }

    // Wrap it in a shared_ptr
    shared_ptr<Mat> frame = make_shared<Mat>(testImage);

    // Create Detector instance
    Detector detector;
    bool is_cuda = false;  // or true, depending on your setup
    detector.init(is_cuda);

    // Perform detection
    detector.detect(frame, true);

    // Get output
    const auto &output = detector.getOutput();

    // Check if output is not empty
    ASSERT_FALSE(output.empty());

    // Print results for manual inspection
    int peopleCount = 0, carCount = 0;
    for (const auto &detection : output) {
        LogManager::logInfoMessage(
            InfoType::DETECTION,
            "ID: " + to_string(detection.id) +
                " Type: " + to_string(detection.type) +
                " Position: " + rectToString(detection.position));
        // Assuming ObjectType::CAR corresponds to the type of car in the enum
        if (detection.type == ObjectType::CAR) {
            carCount++;
        }
        else
            peopleCount++;
    }

    // Assert that exactly 3 cars are detected
    ASSERT_EQ(carCount, 3);
    ASSERT_EQ(peopleCount, 0);
}

// Test case for Detector::detect with a real image containing 2 cars
TEST(DetectorTest, DetectTwoPeoples)
{
    // Load a real image from file
    string imagePath = "../tests/images/two_peoples_image.jpg";
    Mat testImage;
    try {
        testImage = loadImage(imagePath);
    }
    catch (const runtime_error &e) {
        LogManager::logErrorMessage(ErrorType::IMAGE_ERROR, string(e.what()));
        FAIL() << e.what();
    }

    // Wrap it in a shared_ptr
    shared_ptr<Mat> frame = make_shared<Mat>(testImage);

    // Create Detector instance
    Detector detector;
    bool is_cuda = false;  // or true
    detector.init(is_cuda);

    // Perform detection
    detector.detect(frame, true);

    // Get output
    const auto &output = detector.getOutput();

    // Check if output is not empty
    ASSERT_FALSE(output.empty());

    // Print results for manual inspection
    int peopleCount = 0, carCount = 0;
    for (const auto &detection : output) {
        LogManager::logInfoMessage(
            InfoType::DETECTION,
            "ID: " + to_string(detection.id) +
                " Type: " + to_string(detection.type) +
                " Position: " + rectToString(detection.position));
        // Assuming ObjectType::CAR corresponds to the type of car in the enum
        if (detection.type == ObjectType::CAR) {
            peopleCount++;
        }
        else
            peopleCount++;
    }

    // Assert that exactly 2 peples are detected
    ASSERT_EQ(carCount, 0);
    ASSERT_EQ(peopleCount, 2);
}

TEST(DetectorTest, DetectChangesTest)
{
    Mat first, second;
    string imagePath1 = "../tests/images/track_2_cars_first_frame.jpg";
    first = loadImage(imagePath1);
    if (first.empty()) {
        LogManager::logErrorMessage(ErrorType::IMAGE_ERROR,
                                    "Could not load image");
        throw runtime_error("Could not open or find the image");
    }
    string imagePath2 = "../tests/images/track_2_cars_second_frame.jpg";
    second = loadImage(imagePath2);
    if (second.empty()) {
        LogManager::logErrorMessage(ErrorType::IMAGE_ERROR,
                                    "Could not load image");
        throw runtime_error("Could not open or find the image");
    }

    // Wrap it in a shared_ptr
    shared_ptr<Mat> firstFrame = make_shared<Mat>(first);
    shared_ptr<Mat> secondFrame = make_shared<Mat>(second);
    // preper detectAll and detectChanges
    //  Create Detector instance
    Detector detectAll;
    Detector detectChanges;
    bool is_cuda = false;  // or true
    detectAll.init(is_cuda);
    detectChanges.init(is_cuda);
    // Perform detection
    detectAll.detect(secondFrame, false);
    detectChanges.detect(firstFrame, false);
    detectChanges.detect(secondFrame, false);
    // Get output
    const auto &detectAllOutput = detectAll.getOutput();
    const auto &detectChangesOutput = detectChanges.getOutput();
    // Check if output is not empty
    ASSERT_FALSE(detectAllOutput.empty());
    ASSERT_FALSE(detectChangesOutput.empty());
    // check if two ouptputs have the same length
    ASSERT_EQ(detectAllOutput.size(), detectChangesOutput.size());
}