#ifndef __CONDITION_FACTORY_H__
#define __CONDITION_FACTORY_H__

#include "or_operator.h"
#include "and_operator.h"
#include "basic_condition.h"

 Condition* createCondition(OperatorTypes operatorType);


#endif // _CONDITION_FACTORY_H_
