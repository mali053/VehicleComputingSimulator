#include "basic_condition.h"

// Implement the functions for print- print myself

void BasicCondition::print() const
{
	cout << "Basic Condition: \ncondition: " << " " << this->operatorType << " " << this->value;
	cout << " status: " << this->status << endl;
}

void BasicCondition::printNice() const
{
	cout << " " << this->operatorType << " " << this->value;
}
