#include "basic_condition.h"

void BasicCondition::setValue(string valueStr, FieldType type)
{
    GlobalProperties::controlLogger.logMessage(logger::LogLevel::DEBUG, "SetValue in BasicCondition to " + valueStr);
    switch (type) {
        case FieldType::UNSIGNED_INT: {
            unsigned int *uintPtr = new unsigned int();
            try {
                *uintPtr = stoi(valueStr);
            }
            catch (const exception &e) {
                cerr << "Error: " << e.what() << endl;
            }
            value = static_cast<void *>(uintPtr);
            break;
        }
        case FieldType::SIGNED_INT: {
            int *intPtr = new int();
            try {
                *intPtr = stoi(valueStr);
            }
            catch (const exception &e) {
                cerr << "Error: " << e.what() << endl;
            }
            value = static_cast<void *>(intPtr);
            break;
        }
        case FieldType::CHAR_ARRAY: {
            std::string *buffer = new std::string(valueStr);
            value = static_cast<void *>(buffer);
            break;
        }
        case FieldType::FLOAT_FIXED: {
            float *floatPtr = new float();
            try {
                *floatPtr = stof(valueStr);
            }
            catch (const exception &e) {
                cerr << "Error: " << e.what() << endl;
            }
            value = static_cast<void *>(floatPtr);
            break;
        }
        case FieldType::FLOAT_MANTISSA: {
            float *floatPtr = new float();
            try {
                *floatPtr = stof(valueStr);
            }
            catch (const exception &e) {
                cerr << "Error: " << e.what() << endl;
            }
            value = static_cast<void *>(floatPtr);
            break;
        }
        case FieldType::BIT_FIELD: {
            unsigned long bitValue;
            try {
                bitValue = stoul(valueStr, nullptr, 2);
            }
            catch (const exception &e) {
                cerr << "Error: " << e.what() << endl;
            }
            unsigned long *bitPtr = &bitValue;
            value = static_cast<void *>(bitPtr);
        }
        case FieldType::DOUBLE: {
            double *doublePtr = new double();
            try {
                *doublePtr = stod(valueStr);
            }
            catch (const exception &e) {
                cerr << "Error: " << e.what() << endl;
            }
            value = static_cast<void *>(doublePtr);
            break;
        }
        default:
            break;
    }
}