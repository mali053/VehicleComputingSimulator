#pragma once
#include <map>
#include <string>
#include "operator_node.h"
using namespace std;

// Class that present a leaf - basic condition
class BasicCondition : public Condition {
public:
	string value;
	string operatorType;

	// Functions for print
	void print() const override;
	void printNice() const override;
};