#include "operator_types.h"

// Function that take a string and return the corresponding operator type
OperatorTypes convertStringToOperatorTypes(string operatorType)
{
    // Define a map to convert from string to operator type
    static unordered_map<string, OperatorTypes> map = {
        {"|", OperatorTypes::o},   {"&", OperatorTypes::a},
        {">", OperatorTypes::b},   {"<", OperatorTypes::l},
        {"=", OperatorTypes::e},   {"!=", OperatorTypes::ne},
        {">=", OperatorTypes::be}, {"<=", OperatorTypes::le}};

    // Find the right type in the map and return it
    auto it = map.find(operatorType);
    return ((it != map.end()) ? (it->second)
                              : (OperatorTypes::UnknownOperatorType));
}