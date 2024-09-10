#include <opencv2/opencv.hpp>
#include <gtest/gtest.h>
#include "detector.h"

bool is_cuda = false;  // or true

// Helper function to load an image from a file
cv::Mat loadImage(const std::string &filename)
{
    cv::Mat img = cv::imread(filename);
    if (img.empty()) {
        throw std::runtime_error("Could not open or find the image");
    }
    return img;
}

// Test case for Detector::detect with a real image containing 2 cars
TEST(DetectorTest, DetectTwoCars)
{
    // Load a real image from file
    std::string imagePath = "../tests/images/two_cars_image.png";
    cv::Mat testImage;
    try {
        testImage = loadImage(imagePath);
    }
    catch (const std::runtime_error &e) {
        FAIL() << e.what();
    }

    // Wrap it in a shared_ptr
    std::shared_ptr<cv::Mat> frame = std::make_shared<cv::Mat>(testImage);

    // Create Detector instance
    Detector detector;

    // bool is_cuda = false; // or true, depending on your setup
    detector.init(is_cuda);

    // Perform detection
    detector.detect(frame);

    // Get output
    const auto &output = detector.getOutput();

    // Check if output is not empty
    ASSERT_FALSE(output.empty());

    // Print results for manual inspection
    int peopleCount = 0, carCount = 0;
    for (const auto &detection : output) {
        std::cout << "Detection ID: " << detection.id
                  << ", Type: " << detection.type
                  << ", Confidence: " << detection.confidence
                  << ", Position: " << detection.position << std::endl;
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
    std::string imagePath = "../tests/images/three_cars_image.png";
    cv::Mat testImage;
    try {
        testImage = loadImage(imagePath);
    }
    catch (const std::runtime_error &e) {
        FAIL() << e.what();
    }

    // Wrap it in a shared_ptr
    std::shared_ptr<cv::Mat> frame = std::make_shared<cv::Mat>(testImage);

    // Create Detector instance
    Detector detector;
    bool is_cuda = false;  // or true, depending on your setup
    detector.init(is_cuda);

    // Perform detection
    detector.detect(frame);

    // Get output
    const auto &output = detector.getOutput();

    // Check if output is not empty
    ASSERT_FALSE(output.empty());

    // Print results for manual inspection
    int peopleCount = 0, carCount = 0;
    for (const auto &detection : output) {
        std::cout << "Detection ID: " << detection.id
                  << ", Type: " << detection.type
                  << ", Confidence: " << detection.confidence
                  << ", Position: " << detection.position << std::endl;
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
    std::string imagePath = "../tests/images/two_peoples_image.jpg";
    cv::Mat testImage;
    try {
        testImage = loadImage(imagePath);
    }
    catch (const std::runtime_error &e) {
        FAIL() << e.what();
    }

    // Wrap it in a shared_ptr
    std::shared_ptr<cv::Mat> frame = std::make_shared<cv::Mat>(testImage);

    // Create Detector instance
    Detector detector;
    bool is_cuda = false;  // or true
    detector.init(is_cuda);

    // Perform detection
    detector.detect(frame);

    // Get output
    const auto &output = detector.getOutput();

    // Check if output is not empty
    ASSERT_FALSE(output.empty());

    // Print results for manual inspection
    int peopleCount = 0, carCount = 0;
    for (const auto &detection : output) {
        std::cout << "Detection ID: " << detection.id
                  << ", Type: " << detection.type
                  << ", Confidence: " << detection.confidence
                  << ", Position: " << detection.position << std::endl;
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

TEST(DetectChangesTest, detect)
{
    cv::Mat first, second;
    //load video
    cv::VideoCapture capture("../tests/images/cars4.mp4");
    if (!capture.isOpened()) {
        throw std::runtime_error("Error while opening video media\n");
    }
    //load first frame from video
    capture.read(first);
    if (first.empty()) {
        throw std::runtime_error("CMedia finished\n");
    }
    //load second frame from video
    capture.read(second);
    if (second.empty()) {
        throw std::runtime_error("CMedia finished\n");
    }
    // Wrap it in a shared_ptr
    std::shared_ptr<cv::Mat> firstFrame = std::make_shared<cv::Mat>(first);
    std::shared_ptr<cv::Mat> secondFrame = std::make_shared<cv::Mat>(second);
    //preper detectAll and detectChanges
    // Create Detector instance
    Detector detectAll;
    Detector detectChanges;
    bool is_cuda = false;  // or true
    detectAll.init(is_cuda);
    detectChanges.init(is_cuda);
    // Perform detection
    detectAll.detect(secondFrame);
    detectChanges.detect(firstFrame);
    detectChanges.detect(secondFrame);
    // Get output
    const auto &detectAllOutput = detectAll.getOutput();
    const auto &detectChangesOutput = detectChanges.getOutput();
    // Check if output is not empty
    ASSERT_FALSE(detectAllOutput.empty());
    ASSERT_FALSE(detectChangesOutput.empty());
    //check if two ouptputs have the same length
    ASSERT_EQ(detectAllOutput.size(), detectChangesOutput.size());
}