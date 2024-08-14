 #include "condition_factory.h"
 
 Condition* createCondition(OperatorTypes operatorType)
 {
    Condition* conditionPtr;
	if (operatorType == OperatorTypes::o)
		conditionPtr = new OrOperator;
	else if (operatorType == OperatorTypes::a)
		conditionPtr = new AndOperator;
	else
		conditionPtr = new BasicCondition();
    return conditionPtr;  
 }