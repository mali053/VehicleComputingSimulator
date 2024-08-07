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

void AndOperator::print() const
{
    cout << "AndOperator: \n" << "status: " << this->status << "{" << endl;
    for (Condition* condition : this->conditions)
        condition->print();
    cout << "}" << endl;
}

void AndOperator::printNice() const
{
    cout << "(";
    for (Condition* condition : this->conditions) {
        condition->printNice();
        cout << " && ";
    }
    cout << ")";
}

