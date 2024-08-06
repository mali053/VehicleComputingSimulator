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

void OrOperator::print() const
{
    cout << "OrOperator: \n" << "status: " << this->status << "\n{" << endl;
    for (Condition* condition : this->conditions)
        condition->print();
    cout << "}" << endl;
}

void OrOperator::printNice() const
{
    cout << "(";
    for (Condition* condition : this->conditions) {
        condition->printNice();
        cout << " || ";
    }
    cout << ")";
}

