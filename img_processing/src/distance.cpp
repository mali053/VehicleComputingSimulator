#define MIN_LEGAL_HEIGHT 900

#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdexcept>
#include "distance.h"
#include "detector.h"
#include "manager.h"

using namespace std;
using namespace cv;

Distance *Distance::instance = nullptr;

Distance::Distance(const cv::Mat &image)
{
    findFocalLength(image);
}

Distance &Distance::getInstance(const cv::Mat &image)
{
    if (!instance) {
        if (image.empty()) {
            LogManager::logErrorMessage(ErrorType::IMAGE_ERROR,
                                        "Could not load image");
            throw std::runtime_error(
                "Could not load image. Distance instance creation failed.");
        }
        else
            instance = new Distance(image);
    }
    return *instance;
}

void Distance::setFocalLength(double focalLength)
{
    this->focalLength = focalLength;
}

// function that finds the distance of the objects and update them in
// detectionObjects
void Distance::findDistance(std::vector<ObjectInformation> &objectInformations)
{
    int knownSize, imageSize;
    // Move over all objects
    for (ObjectInformation &objectInformation : objectInformations) {
        // is person
        if (objectInformation.type == 0) {
            // distance test based on the position of the legs in the image
            if (objectInformation.position.y +
                    objectInformation.position.height >
                MIN_LEGAL_HEIGHT) {
                addDistance(0, objectInformation);
                return;
            }
            // Find the size of the object in reality and in the picture
            knownSize = PERSON_HEIGHT;
            imageSize = objectInformation.position.height;
        }
        else {
            // Find the size of the object in reality and in the picture
            knownSize = CAR_WIDTH;
            imageSize = objectInformation.position.width;
        }
        // Calculate the distance in meters
        float distance = (focalLength * knownSize / imageSize) / 1000;
        addDistance(distance, objectInformation);
    }
}

void Distance::findFocalLength(const cv::Mat &image)
{
    // Check if the input image is empty
    if (image.empty()) {
        LogManager::logErrorMessage(ErrorType::IMAGE_ERROR,
                                    "Could not load image");
        //throw std::runtime_error("Could not open or find the image");
        return;
    }

    // Convert the input image to grayscale
    Mat testImage, grayImage;
    image.copyTo(testImage);
    cvtColor(testImage, grayImage, COLOR_BGR2GRAY);
    grayImage = 255 - grayImage;

    // Apply thresholding to the grayscale image
    Mat thresh;
    threshold(grayImage, thresh, 0, 255, THRESH_BINARY | THRESH_OTSU);

    // Find contours in the thresholded image
    std::vector<std::vector<Point>> contours;
    findContours(thresh, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // Find the longest contour (assumed to be the black line)
    double maxLength = 0.0;
    std::vector<cv::Point> maxContour;
    for (const auto &contour : contours) {
        double length = cv::arcLength(contour, true);
        if (length > maxLength) {
            maxLength = length;
            maxContour = contour;
        }
    }

    // Find the top-left and bottom-right points of the bounding box
    cv::Point topLeft = maxContour[0];
    cv::Point bottomRight = maxContour[0];
    for (const auto &point : maxContour) {
        if (point.y < topLeft.y ||
            (point.y == topLeft.y && point.x < topLeft.x)) {
            topLeft = point;
        }
        if (point.y > bottomRight.y ||
            (point.y == bottomRight.y && point.x > bottomRight.x)) {
            bottomRight = point;
        }
    }

    // Calculate the width of the bounding box
    double rectWidth = sqrt(pow(bottomRight.y - topLeft.y, 2) +
                            pow(bottomRight.x - topLeft.x, 2));

    // Known parameters
    const double actualLineLengthMm = 200.0;
    const double distanceToCameraMm = 1000.0;

    // Calculate focal length in pixels
    this->focalLength = (rectWidth * distanceToCameraMm) / actualLineLengthMm;
}

void Distance::addDistance(float distance, ObjectInformation &obj)
{
    if (obj.prevDistances.size() == MAX_PREV_DISTANCES_SIZE)
        obj.prevDistances.pop_front();
    obj.prevDistances.push_back(distance);
    obj.distance = distance;
}