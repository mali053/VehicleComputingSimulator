#pragma once
#include <map>
#include <string>
#include "operator_node.h"
using namespace std;

// Class that present a leaf - basic condition
class BasicCondition : public Condition {
public:
	int conditionId;
	string value;
	string operatorType;

	// C-tor that gets an id and initializes the conditionId
	BasicCondition(int id) : conditionId(id) {};

	// Functions for print
	void print() const override;
	void printNice() const override;
};