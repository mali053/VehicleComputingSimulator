#include "and_operator.h"

void AndOperator::updateStatus()
{
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

