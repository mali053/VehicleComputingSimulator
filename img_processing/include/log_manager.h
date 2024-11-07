#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <string>
#include <unordered_map>
#include <variant>
#include "logger.h"

enum class ErrorType {
    IMAGE_ERROR,
    VIDEO_ERROR,
    FILE_ERROR,
    DETECTION_ERROR,
    TRACKING_ERROR,
    MODEL_ERROR
};

enum class InfoType {
    MEDIA_FINISH,
    ALERT_SENT,
    CHANGED,
    UNION,
    NUM_OF_DIFFERENCE,
    EXECUTION_TIME,
    IOU,
    DETECTION,
    TRACKING,
    DISTANCE,
    MODE
};

enum class DebugType { PRINT };

class LogManager {
   public:
    static logger imgLogger;

    template <typename T>
    static void logErrorMessage(T errorType, const std::string &details = "");

    template <typename T>
    static void logInfoMessage(T infoType, const std::string &details = "");

    template <typename T>
    static void logDebugMessage(T debugType, const std::string &details = "");

   private:
    template <typename T>
    static std::string enumTypeToString(T enumType);
};

#endif  // LOG_MANAGER_H