#include "log_manager.h"

using namespace std;

logger LogManager::imgLogger("img_processing");

using EnumVariant = std::variant<ErrorType, InfoType, DebugType>;

template <typename T>
string LogManager::enumTypeToString(T enumType)
{
    static const std::unordered_map<EnumVariant, std::string>
        enumTypeToStringMap = {
            {ErrorType::IMAGE_ERROR, "IMAGE_ERROR"},
            {ErrorType::VIDEO_ERROR, "VIDEO_ERROR"},
            {ErrorType::FILE_ERROR, "FILE_ERROR"},
            {ErrorType::DETECTION_ERROR, "DETECTION_ERROR"},
            {ErrorType::TRACKING_ERROR, "TRACKING_ERROR"},
            {ErrorType::MODEL_ERROR, "MODEL_ERROR"},
            {InfoType::MEDIA_FINISH, "MEDIA_FINISH"},
            {InfoType::ALERT_SENT, "ALERT_SENT"},
            {InfoType::CHANGED, "CHANGED"},
            {InfoType::UNION, "UNION"},
            {InfoType::NUM_OF_DIFFERENCE, "NUM_OF_DIFFERENCE"},
            {InfoType::EXECUTION_TIME, "EXECUTION_TIME"},
            {InfoType::IOU, "IOU"},
            {InfoType::DETECTION, "DETECTION"},
            {InfoType::TRACKING, "TRACKING"},
            {InfoType::DISTANCE, "DISTANCE"},
            {InfoType::MODE, "MODE"},
            {DebugType::PRINT, "PRINT"}};
    auto it = enumTypeToStringMap.find(enumType);
    if (it != enumTypeToStringMap.end()) {
        return it->second;
    }
    else {
        return "UNKNOWN";
    }
}

template <typename T>
void LogManager::logErrorMessage(T errorType, const string &details)
{
    string messageText = enumTypeToString(errorType);
    if (!details.empty()) {
        messageText += ": " + details;
    }
    LogManager::imgLogger.logMessage(logger::LogLevel::ERROR, messageText);
}

template <typename T>
void LogManager::logInfoMessage(T infoType, const string &details)
{
    string messageText = enumTypeToString(infoType);
    if (!details.empty()) {
        messageText += ": " + details;
    }
    LogManager::imgLogger.logMessage(logger::LogLevel::INFO, messageText);
}

template <typename T>
void LogManager::logDebugMessage(T debugType, const string &details)
{
    string messageText = enumTypeToString(debugType);
    if (!details.empty()) {
        messageText += ": " + details;
    }
    LogManager::imgLogger.logMessage(logger::LogLevel::DEBUG, messageText);
}

// Explicit instantiation for ErrorType
template void LogManager::logErrorMessage<ErrorType>(ErrorType errorType,
                                                     const string &details);

// Explicit instantiation for InfoType
template void LogManager::logInfoMessage<InfoType>(InfoType infoType,
                                                   const string &details);

// Explicit instantiation for DebugType
template void LogManager::logDebugMessage<DebugType>(DebugType debugType,
                                                     const string &details);
