#include "basic_condition.h"

void BasicCondition::setValue(string valueStr, FieldType type)
{
    GlobalProperties::controlLogger.logMessage(logger::LogLevel::DEBUG, "SetValue in BasicCondition to " + valueStr);
    switch (type) {
        case FieldType::UNSIGNED_INT: {
            unsigned int uintVal;
            try {
                uintVal = stoi(valueStr);
            }
            catch (const exception &e) {
                GlobalProperties::controlLogger.logMessage(logger::LogLevel::ERROR, e.what());
            }
            value = uintVal;
            break;
        }
        case FieldType::SIGNED_INT: {
            int intVal;
            try {
                intVal = stoi(valueStr);
            }
            catch (const exception &e) {
                GlobalProperties::controlLogger.logMessage(logger::LogLevel::ERROR, e.what());
            }
            value = intVal;
            break;
        }
        case FieldType::CHAR_ARRAY: {
            value = valueStr;
            break;
        }
        case FieldType::FLOAT_FIXED: {
            float floatVal;
            try {
                floatVal = stof(valueStr);
            }
            catch (const exception &e) {
                GlobalProperties::controlLogger.logMessage(logger::LogLevel::ERROR, e.what());
            }
            value = floatVal;
            break;
        }
        case FieldType::FLOAT_MANTISSA: {
            float floatVal;
            try {
                floatVal = stof(valueStr);
            }
            catch (const exception &e) {
                GlobalProperties::controlLogger.logMessage(logger::LogLevel::ERROR, e.what());
            }
            value = floatVal;
            break;
        }
        case FieldType::BOOLEAN: {
            bool boolVal = (valueStr == "true") ? true : false;
            value = boolVal;
            break;
        }
        case FieldType::DOUBLE: {
            double doubleVal;
            try {
                doubleVal = stod(valueStr);
            }
            catch (const exception &e) {
                GlobalProperties::controlLogger.logMessage(logger::LogLevel::ERROR, e.what());
            }
            value = doubleVal;
            break;
        }
        default: {
            GlobalProperties::controlLogger.logMessage(logger::LogLevel::ERROR, "Invalid FieldType encountered");
            break;
        }
    }
}