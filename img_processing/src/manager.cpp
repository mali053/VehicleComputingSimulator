#define NUM_OF_TRACKING 10

#include "manager.h"
#include "alert.h"

using namespace std;
using namespace cv;

void processData(uint32_t srcId, void *data) {}

Manager::Manager(int processID)
    : processID(processID), communication(processID, processData)
{
}

void Manager::init()
{
    string message = "Hello, I'm img_processing " + to_string(processID) +
                     " sending to process " + to_string(destID);
    size_t dataSize = message.length() + 1;
    destID = 1;
    // Starting communication with the server
    communication.startConnection();
    // Sending the message
    communication.sendMessage((void *)message.c_str(), dataSize, destID,
                              processID, false);

    // calibration
    Mat calibrationImage = imread("../tests/images/black_line.JPG");
    if (calibrationImage.empty()) {
        LogManager::logErrorMessage(ErrorType::IMAGE_ERROR, "image not found");
        return;
    }
    Distance &distance = Distance::getInstance(calibrationImage);
    iterationCnt = 1;
    bool isCuda = false;
    detector.init(isCuda);
    dynamicTracker.init();
    velocity.init(0.04);
}

void Manager::mainDemo()
{
    string filePath = "../data.txt";
    // open the file
    ifstream file(filePath);
    if (!file.is_open()) {
        LogManager::logErrorMessage(ErrorType::FILE_ERROR);
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
            LogManager::logErrorMessage(ErrorType::VIDEO_ERROR);
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
        LogManager::logErrorMessage(ErrorType::VIDEO_ERROR, "video not found");
        throw runtime_error("video not found");
        return;
    }
    while (1) {
        capture >> frame;
        if (frame.empty()) {
            LogManager::logInfoMessage(InfoType::MEDIA_FINISH);
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
        dynamicTracker.tracking(this->currentFrame, this->currentOutput);
    }

    // add distance to detection objects
    distance.findDistance(this->currentOutput);
    velocity.returnVelocities(this->currentOutput);

    // send allerts to main control
    vector<vector<uint8_t>> alerts = alerter.sendAlerts(this->currentOutput);
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
        int topLeftX = objectInformation.position.x;
        int topLeftY = objectInformation.position.y;

        // Draw rectangle around object
        Scalar boxColor =
            (objectInformation.distance < (alerter.MIN_LEGAL_DISTANCE))
                ? Scalar(0, 0, 255)
                : Scalar(0, 255, 0);
        rectangle(*currentFrame, objectInformation.position, boxColor, 2);

        // Define text for distance and velocity
        std::stringstream ssDistance, ssVelocity;
        ssDistance << std::fixed << std::setprecision(2)
                   << objectInformation.distance;
        ssVelocity << std::fixed << std::setprecision(2)
                   << objectInformation.velocity;

        std::string distanceText = ssDistance.str();
        std::string velocityText = ssVelocity.str();

        // Font properties
        int fontFace = FONT_HERSHEY_SIMPLEX;
        double fontScale = 0.6;
        int thickness = 1;
        int baseline = 0;

        // Calculate text sizes
        Size distanceTextSize = getTextSize(distanceText, fontFace, fontScale,
                                            thickness, &baseline);
        Size velocityTextSize = getTextSize(velocityText, fontFace, fontScale,
                                            thickness, &baseline);

        // Positions for the texts
        Point distanceTextOrg(topLeftX + 5, topLeftY - velocityTextSize.height -
                                                7);         // Above the object
        Point velocityTextOrg(topLeftX + 5, topLeftY - 5);  // Above the object

        // Draw outline for distance text
        putText(*currentFrame, distanceText, distanceTextOrg, fontFace,
                fontScale, Scalar(0, 0, 0), thickness + 2);
        // Write the distance text
        putText(*currentFrame, distanceText, distanceTextOrg, fontFace,
                fontScale, Scalar(255, 255, 255), thickness);

        // Draw outline for velocity text
        putText(*currentFrame, velocityText, velocityTextOrg, fontFace,
                fontScale, Scalar(0, 0, 0), thickness + 2);
        // Write the velocity text
        putText(*currentFrame, velocityText, velocityTextOrg, fontFace,
                fontScale, Scalar(255, 0, 0), thickness);
    }

    // Legend
    int legendX = 10, legendY = 10;
    putText(*currentFrame, "Legend:", Point(legendX, legendY),
            FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 1);
    rectangle(*currentFrame, Point(legendX, legendY + 10),
              Point(legendX + 10, legendY + 30), Scalar(255, 255, 255), FILLED);
    putText(*currentFrame, "Distance", Point(legendX + 15, legendY + 25),
            FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 1);
    rectangle(*currentFrame, Point(legendX, legendY + 35),
              Point(legendX + 10, legendY + 55), Scalar(255, 0, 0), FILLED);
    putText(*currentFrame, "velocity", Point(legendX + 15, legendY + 50),
            FONT_HERSHEY_SIMPLEX, 0.6, Scalar(255, 255, 255), 1);
}

void Manager::sendAlerts(vector<vector<uint8_t>> &alerts)
{
    for (std::vector<uint8_t> &alertBuffer : alerts) {
        communication.sendMessage(alertBuffer.data(), alertBuffer.size(),
                                  destID, processID, false);
    }
}

void Manager::prepareForTheNext()
{
    prevFrame = currentFrame;
}