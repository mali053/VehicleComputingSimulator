#ifndef __OPERATOR_TYPES_H__
#define __OPERATOR_TYPES_H__

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

#endif  // __OPERATOR_TYPES_H__
