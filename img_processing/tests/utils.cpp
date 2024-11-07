#include "utils.h"

std::string rectToString(const cv::Rect &rect)
{
    return "Rect(x: " + std::to_string(rect.x) +
           ", y: " + std::to_string(rect.y) +
           ", width: " + std::to_string(rect.width) +
           ", height: " + std::to_string(rect.height) + ")";
}