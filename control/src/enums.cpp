#include "enums.h"

// Function that take a string and return the corresponding operator type
OperatorTypes convertStringToOperatorTypes(string operatorType)
{
    // Define a map to convert from string to operator type
    unordered_map<string, OperatorTypes> map = {
        {"|", OperatorTypes::o},   {"&", OperatorTypes::a},
        {">", OperatorTypes::b},   {"<", OperatorTypes::l},
        {"=", OperatorTypes::e},   {"!=", OperatorTypes::ne},
        {">=", OperatorTypes::be}, {"<=", OperatorTypes::le}};

    // Find the right type in the map and return it
    auto it = map.find(operatorType);
    return ((it != map.end()) ? (it->second)
                              : (OperatorTypes::UnknownOperatorType));
}

// Function that take a string and return the corresponding sensor type
SensorsTypes convertStringToSensorsTypes(string type)
{
    // Define a map to convert from string to sensor type
    static unordered_map<string, SensorsTypes> s_map = {
        {"Speed", SensorsTypes::Speed},
        {"Tire Pressure", SensorsTypes::TirePressure},
        {"Light", SensorsTypes::Light}};

    // Find the right type in the map and return it
    auto it = s_map.find(type);
    return ((it == s_map.end()) ? (SensorsTypes::UnknownSensorType)
                                : (it->second));
}