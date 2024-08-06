#pragma once
#include "operator_node.h"

class OrOperator : public OperatorNode {
public:
	//Updates the status according to the type of operator
	void updateStatus() override;

	//Functions for print the tree
	void print() const override;
	void printNice() const override;
};