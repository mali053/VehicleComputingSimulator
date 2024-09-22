#define NUM_OF_TRACKING 10

#include "manager.h"
#include "alert.h"

using namespace std;
using namespace cv;

logger Manager::imgLogger("img_processing");

void Manager::init()
{
    // calibration
    Mat calibrationImage = imread("../tests/images/black_line.JPG");
    if (calibrationImage.empty()) {
        Manager::imgLogger.logMessage(logger::LogLevel::ERROR,
                                      "image not found");
        return;
    }
    Distance &distance = Distance::getInstance(calibrationImage);
    iterationCnt = 1;
    bool isCuda = false;
    detector.init(isCuda);
    dynamicTracker.init();
}

void Manager::mainDemo()
{
    string filePath = "../data.txt";
    // open the file
    ifstream file(filePath);
    if (!file.is_open()) {
        Manager::imgLogger.logMessage(logger::LogLevel::ERROR,
                                      "Error opening file.");
        return;
    }
    string line;
    // run over the file and read the lines
    while (getline(file, line)) {
        // intialize the iteration cnt
        iterationCnt = 1;
        istringstream iss(line);
        string videoPath;
        double focalLength;
        // read the parameters
        if (getline(iss, videoPath, '|') && iss >> focalLength) {
            // Trim leading and trailing whitespaces from videoPath
            videoPath.erase(0, videoPath.find_first_not_of(" \t\n\r\f\v"));
            videoPath.erase(videoPath.find_last_not_of(" \t\n\r\f\v") + 1);
        }
        else {
            Manager::imgLogger.logMessage(logger::LogLevel::ERROR,
                                          "Error parsing line");
            return;
        }
        // intialize focal length
        Distance &distance = Distance::getInstance();
        distance.setFocalLength(focalLength);
        runOnVideo(videoPath);
    }
    cout << "finish reading data";
}

void Manager::runOnVideo(string videoPath)
{
    // Convert Windows file path to WSL file path format
    if (videoPath.length() >= 3 && videoPath[1] == ':') {
        // Convert to lowercase
        char driveLetter = tolower(static_cast<unsigned char>(videoPath[0]));
        videoPath = "/mnt/" + string(1, driveLetter) + videoPath.substr(2);
        // Replace backslashes with forward slashes
        replace(videoPath.begin(), videoPath.end(), '\\', '/');
    }
    // open the video
    VideoCapture capture(videoPath);
    Mat frame = Mat::zeros(480, 640, CV_8UC3);
    if (!capture.isOpened()) {
        Manager::imgLogger.logMessage(logger::LogLevel::ERROR,
                                      "Error while opening video media");
        return;
    }
    while (1) {
        capture >> frame;
        if (frame.empty()) {
            Manager::imgLogger.logMessage(logger::LogLevel::INFO,
                                          "media finish");
            break;
        }
        int result = processing(frame, true);
        if (result == -1)
            return;
    }
}

bool Manager::isDetect(bool isTravel)
{
    if (!isTravel || iterationCnt == 1)
        return true;
    return false;
}

bool Manager::isResetTracker(bool isTravel)
{
    if (isTravel && iterationCnt == 1)
        return true;
    return false;
}

bool Manager::isTrack(bool isTravel)
{
    if (isTravel && iterationCnt > 1)
        return true;
    return false;
}

int Manager::processing(const Mat &newFrame, bool isTravel)
{
    Distance &distance = Distance::getInstance();
    currentFrame = make_shared<Mat>(newFrame);
    if (isDetect(isTravel)) {
        // send the frame to detect
        detector.detect(this->currentFrame, isTravel);
        this->currentOutput = detector.getOutput();
    }

    if (isResetTracker(isTravel)) {
        // prepare the tracker
        dynamicTracker.startTracking(this->currentFrame, this->currentOutput);
    }

    if (isTrack(isTravel)) {
        // send the frame to track
        dynamicTracker.tracking(this->currentFrame);
        this->currentOutput = dynamicTracker.getOutput();
    }

    // add distance to detection objects
    distance.findDistance(this->currentOutput);

    // send allerts to main control
    vector<unique_ptr<char>> alerts = alerter.sendAlerts(this->currentOutput);
    sendAlerts(alerts);

    // update of the iterationCnt
    if (isTravel) {
        iterationCnt = iterationCnt % NUM_OF_TRACKING + 1;
    }

    // visual
    drawOutput();
    imshow("aaa", *currentFrame);
    int key = cv::waitKey(1);
    if (key == 27) {
        return -1;
    }
    return 1;
}

void Manager::drawOutput()
{
    for (ObjectInformation objectInformation : currentOutput) {
        int bottomRightX =
            objectInformation.position.x + objectInformation.position.width;
        int bottomRightY =
            objectInformation.position.y + objectInformation.position.height;
        int topLeftX = objectInformation.position.x;
        int topLeftY = objectInformation.position.y;
        rectangle(*currentFrame, objectInformation.position, Scalar(0, 255, 0),
                  2);
        // Define the text to write
        std::string text = to_string(objectInformation.distance);
        // Specify font properties
        int fontFace = FONT_HERSHEY_SIMPLEX;
        double fontScale = 1;
        int thickness = 2;
        int baseline = 0;
        // Get the size of the text
        Size textSize =
            getTextSize(text, fontFace, fontScale, thickness, &baseline);
        // Calculate the position to write the text at the center of the rectangle
        Point textOrg((bottomRightX + topLeftX - textSize.width) / 2,
                      (bottomRightY + topLeftY + textSize.height) / 2);
        // Write the text on the image
        putText(*currentFrame, text, textOrg, fontFace, fontScale,
                Scalar(255, 0, 0), thickness);
    }
}

void Manager::sendAlerts(vector<unique_ptr<char>> &alerts)
{
    for (const std::unique_ptr<char> &alertBuffer : alerts) {
        // use send communication function
    }
}

void Manager::prepareForTheNext()
{
    prevFrame = currentFrame;
}