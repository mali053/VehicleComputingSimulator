#pragma once
#include "operator_node.h"

class AndOperator : public OperatorNode {
public:
	void updateStatus() override;

	void print() const override;
	void printNice() const override;
};