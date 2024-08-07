#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>
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
