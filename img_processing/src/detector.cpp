#include <string>
#include "detector.h"
#include "manager.h"

using namespace std;
using namespace cv;
using namespace dnn;

void Detector::detect(const shared_ptr<Mat> &frame, bool isTravel)
{
    // intialize variables
    idCounter = 0;
    output.clear();
    this->prevFrame = this->currentFrame;
    this->currentFrame = frame;
    // alwais detect regulary
    if (isTravel)
        detectObjects(currentFrame, Point(0, 0));
    // detect just the first frame
    else {
        if (!prevFrame) {
            detectObjects(currentFrame, Point(0, 0));
        }
        else {
            detectChanges();
        }
    }
}

void Detector::detectObjects(const shared_ptr<Mat> &frame,
                             const Point &position)
{
    // Prepare a blob from the input image formatted for YOLOv5
    Mat blob;
    // Custom function to format the input image
    auto inputImage = formatYolov5(frame);

    // Convert the image to a blob suitable for YOLOv5
    blobFromImage(inputImage, blob, 1. / 255., Size(INPUT_WIDTH, INPUT_HEIGHT),
                  Scalar(), true, false);
    // Set the blob as input to the neural network
    net.setInput(blob);

    // Perform a forward pass to get the output from the YOLOv5 model
    vector<Mat> outputs;
    // Obtain the detections
    net.forward(outputs, net.getUnconnectedOutLayersNames());
    // Calculate scaling factors for bounding box adjustments
    float xFactor = inputImage.cols / INPUT_WIDTH;
    float yFactor = inputImage.rows / INPUT_HEIGHT;
    // Pointer to the output data; YOLOv5 outputs 85 values per detection:
    // 0-3 = bounding box position, 4 = confidence, 5-84 = class scores
    float *data = (float *)outputs[0].data;
    // Number of elements per detection
    const int dimensions = 85;
    // Number of detections in the output
    const int rows = 25200;

    // Vectors to hold detection results:
    // class IDs, confidences, and bounding boxes
    vector<int> classIds;
    vector<float> confidences;
    vector<Rect> boxes;
    // Loop through each detection
    for (int i = 0; i < rows; ++i) {
        // Extract confidence score
        float confidence = data[4];
        if (confidence >= CONFIDENCE_THRESHOLD) {
            // Extract class scores starting from the 5th element
            float *classesScores = data + 5;
            Mat scores(1, ObjectType::COUNT, CV_32FC1, classesScores);
            // Find the highest class score and its index (class ID)
            Point classId;
            double maxClassScore;
            minMaxLoc(scores, 0, &maxClassScore, 0, &classId);
            // Save detections that meet the score threshold
            // types and are valid object
            if (maxClassScore > SCORE_THRESHOLD &&
                isValidObjectType(classId.x)) {
                confidences.push_back(confidence);
                classIds.push_back(classId.x);
                // Extract bounding box coordinates (x, y, width, height)
                float x = data[0];
                float y = data[1];
                float w = data[2];
                float h = data[3];
                // Adjust bounding box coordinates with scaling factors
                int left = int((x - 0.5 * w) * xFactor);
                int top = int((y - 0.5 * h) * yFactor);
                int width = int(w * xFactor);
                int height = int(h * yFactor);
                // Add the bounding box to the list
                boxes.push_back(Rect(left, top, width, height));
            }
        }

        // Move to the next detection (next 85 elements)
        data += dimensions;
    }
    // Perform non-maximum suppression to filter overlapping boxes
    vector<int> nmsResult;
    NMSBoxes(boxes, confidences, SCORE_THRESHOLD, NMS_THRESHOLD, nmsResult);
    // Loop through the results of NMS to create the final list of detections
    for (int i = 0; i < nmsResult.size(); i++) {
        int idx = nmsResult[i];
        // Create a objectInformation for each valid detection
        ObjectInformation result;
        // The conversion may fail because the model is trained to identify
        // different objects A model may be identified with a number greater than 2
        // While ObjectType Only keeps 3 organs
        result.type = static_cast<ObjectType>(classIds[idx]);
        result.position = boxes[idx];
        result.id = idCounter++;
        output.push_back(result);
    }
}

void Detector::detectChanges()
{
    const vector<Rect> changedAreas = findDifference();
    LogManager::logDebugMessage(DebugType::PRINT,
                                "Areas- " + changedAreas.size());
    for (Rect oneChange : changedAreas) {
        int x = oneChange.x;
        int y = oneChange.y;
        Point position(x, y);
        Mat view(*currentFrame, oneChange);
        detectObjects(make_shared<Mat>(view), position);
    }
}

vector<Rect> Detector::findDifference()
{
    vector<Rect> differencesRects;
    Mat prevGray, currentGray;
    cvtColor(*prevFrame, prevGray, COLOR_BGR2GRAY);
    cvtColor(*currentFrame, currentGray, COLOR_BGR2GRAY);
    // Find the difference between the two images
    Mat diff;
    absdiff(prevGray, currentGray, diff);
    // Apply threshold
    Mat thresh;
    threshold(diff, thresh, 0, 255, THRESH_BINARY | THRESH_OTSU);
    // Dilation
    Mat kernel = Mat::ones(5, 5, CV_8U);
    Mat dil;
    dilate(thresh, dil, kernel, Point(-1, -1), 2);
    // Calculate contours
    std::vector<std::vector<Point>> contours;
    findContours(dil, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    for (const auto &contour : contours) {
        // Calculate bounding box around contour
        Rect boundingBox = boundingRect(contour);
        differencesRects.push_back(boundingBox);
    }
    LogManager::logDebugMessage(DebugType::PRINT,
                                to_string(differencesRects.size()));
    vector<Rect> unionRects = unionOverlappingRectangels(differencesRects);
    LogManager::logDebugMessage(DebugType::PRINT, to_string(unionRects.size()));
    return unionRects;
}

// function that goes through all the rectangles
// and unites rectangles that have overlapping parts
vector<Rect> Detector::unionOverlappingRectangels(vector<Rect> allChanges)
{
    bool isUnion;
    int sizeVec = 0;
    vector<Rect> unionRect;
    Rect temp;
    for (int i = 0; i < allChanges.size(); i++) {
        isUnion = false;
        sizeVec = unionRect.size();
        for (int j = 0; j < sizeVec; j++) {
            temp = unionRect[0];
            unionRect.erase(unionRect.begin());
            if ((allChanges[i] & temp).area() > 0) {
                allChanges[i] |= temp;
            }
            else {
                unionRect.push_back(temp);
            }
        }
        for (int j = i + 1; j < allChanges.size(); j++) {
            if ((allChanges[i] & allChanges[j]).area() > 0) {
                allChanges[j] |= allChanges[i];
                isUnion = true;
                break;
            }
        }
        if (!isUnion) {
            unionRect.push_back(allChanges[i]);
        }
    }
    for (Rect r : unionRect) {
        rectangle(*currentFrame, r, Scalar(0, 0, 255), 2);
    }
    imshow("win", *currentFrame);
    waitKey();
    return unionRect;
}

vector<ObjectInformation> Detector::getOutput() const
{
    return output;
}

// A function that changes the image to fit the model
Mat Detector::formatYolov5(const shared_ptr<Mat> &frame)
{
    int col = frame->cols;
    int row = frame->rows;
    int maximum = MAX(col, row);
    Mat result = Mat::zeros(maximum, maximum, CV_8UC3);
    frame->copyTo(result(Rect(0, 0, col, row)));
    return result;
}

void Detector::init(bool isCuda)
{
    loadNet(isCuda);
}

// Loading yolov5s onnx model
void Detector::loadNet(bool isCuda)
{
    auto result = readNet("../yolov5s.onnx");
    if (result.empty()) {
        LogManager::logErrorMessage(ErrorType::MODEL_ERROR,
                                    "failed to load yolov5");
        throw runtime_error("Could not open or find yolov5 model");
    }

    if (isCuda) {
        LogManager::logInfoMessage(InfoType::MODE, "using CUDA");
        result.setPreferableBackend(DNN_BACKEND_CUDA);
        result.setPreferableTarget(DNN_TARGET_CUDA_FP16);
    }
    else {
        LogManager::logInfoMessage(InfoType::MODE, "CPU");
        result.setPreferableBackend(DNN_BACKEND_OPENCV);
        result.setPreferableTarget(DNN_TARGET_CPU);
    }
    net = result;
}

bool Detector::isValidObjectType(int value) const
{
    switch (value) {
        case CAR:
        case PEOPLE:
            return true;
        default:
            return false;
    }
}
