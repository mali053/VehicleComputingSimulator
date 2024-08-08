#include "detector.h"

using namespace std;
using namespace cv;
using namespace dnn;

void Detector::detect(const shared_ptr<Mat> &frame)
{
    // Store the input frame for processing
    this->frame = frame;
    // Prepare a blob from the input image formatted for YOLOv5
    Mat blob;
    // Custom function to format the input image
    auto inputImage = formatYolov5();
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
        // Check if the confidence is above the threshold to consider the detection
        if (confidence >= CONFIDENCE_THRESHOLD) {
            // Extract class scores starting from the 5th element
            float *classesScores = data + 5;
            Mat scores(1, ObjectType::COUNT, CV_32FC1, classesScores);
            // Find the highest class score and its index (class ID)
            Point classId;
            double maxClassScore;
            minMaxLoc(scores, 0, &maxClassScore, 0, &classId);
            // Save detections that meet the score threshold and are valid object types
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
        // Create a DetectionObject for each valid detection
        DetectionObject result;
        // Set the object type
        result.type = static_cast<ObjectType>(classIds[idx]);
        // Set the confidence score
        result.confidence = confidences[idx];
        // Set the bounding box position
        result.position = boxes[idx];
        // Add the detection result to the output list
        output.push_back(result);
    }
}

vector<DetectionObject> Detector::getOutput() const { return output; }

Mat Detector::formatYolov5() {
  int col = frame->cols;
  int row = frame->rows;
  int maximum = MAX(col, row);
  Mat result = Mat::zeros(maximum, maximum, CV_8UC3);
  frame->copyTo(result(Rect(0, 0, col, row)));
  return result;
}

void Detector::init(bool isCuda) { loadNet(isCuda); }


void Detector::loadNet(bool isCuda)
{
    // Loading yolov5s onnx model
    auto result = readNet("../yolov5s.onnx");
    if (result.empty()) {
        cerr << "failed to load yolov5 model";
    }

    if (isCuda) {
        cout << "Using CUDA\n";
        result.setPreferableBackend(DNN_BACKEND_CUDA);
        result.setPreferableTarget(DNN_TARGET_CUDA_FP16);
    }
    else {
        cout << "CPU Mode\n";
        result.setPreferableBackend(DNN_BACKEND_OPENCV);
        result.setPreferableTarget(DNN_TARGET_CPU);
    }
    net = result;
}

bool Detector::isValidObjectType(int value) const {
  switch (value) {
  case PEOPLE:
  case CAR:
    return true;
  default:
    return false;
  }
}
