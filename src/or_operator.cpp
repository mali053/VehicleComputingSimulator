#include "or_operator.h"

void OrOperator::updateStatus()
{
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

