#ifndef __ENUMS_H__
#define __ENUMS_H__

#include <string>
#include <unordered_map>
using namespace std;

enum OperatorTypes {
    o,   // ||
    a,   // &&
    b,   // >
    l,   // <
    e,   // =
    ne,  // !=
    be,  // >=
    le,  // <=
    UnknownOperatorType
};

// Function that take a string and return the corresponding operator type
OperatorTypes convertStringToOperatorTypes(string operatorType);

// An enum that contains all possible sensor types
enum SensorsTypes { Speed, TirePressure, Light, UnknownSensorType };

// Function that take a string and return the corresponding sensor type
SensorsTypes convertStringToSensorsTypes(string type);

#endif  // _ENUMS_H_
