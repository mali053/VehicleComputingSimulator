
#ifndef __BASIC_CONDITION_H__
#define __BASIC_CONDITION_H__

#include <map>
#include <string>
#include <bitset>
#include "operator_node.h"
#include "operator_types.h"
#include "global_properties.h"

#include "../../parser_json/src/packet_parser.h"

using namespace std;

// Class that present a leaf - basic condition
class BasicCondition : public Condition {
public:
    FieldValue value;
    OperatorTypes operatorType;
    BasicCondition(OperatorTypes ot) : operatorType(ot) {}

    void setValue(string value, FieldType type);
};

#endif  // _BASIC_CONDITION_H_