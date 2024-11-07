#include "and_operator.h"

//Updates the status according to the type of operator
void AndOperator::updateStatus()
{
    //If all the conditions that make up the condition are true
    if (this->countTrueConditions == this->conditions.size())
        this->status = true;
    else
        this->status = false;
}