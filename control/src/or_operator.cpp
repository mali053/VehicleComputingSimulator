#include "or_operator.h"

//Updates the status according to the type of operator
void OrOperator::updateStatus()
{
    //If one of the conditions that make up the condition is true
    if (this->countTrueConditions > 0)
        this->status = true;
    else
        this->status = false;
}