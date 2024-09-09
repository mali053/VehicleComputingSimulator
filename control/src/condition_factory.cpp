#include "condition_factory.h"

// Function that creates and returns a Condition object based on the given OperatorTypes value.
Condition *createCondition(OperatorTypes operatorType)
{
    Condition *conditionPtr;
    if (operatorType == OperatorTypes::o)
        conditionPtr = new OrOperator;
    else if (operatorType == OperatorTypes::a)
        conditionPtr = new AndOperator();
    else
        conditionPtr = new BasicCondition();
    return conditionPtr;
}