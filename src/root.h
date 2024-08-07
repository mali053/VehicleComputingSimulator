#pragma once
#include "node.h"
#include "global_properties.h"

class Root : public Node {
public:
	int conditionId;
	Condition* firstCondition;

	// C-tor that gets an id and initializes the conditionId
	Root(int id, Condition* firstCondition) : conditionId(id) {
		this->firstCondition = firstCondition;
	}


	// Function that updates the status of all nodes that should change
	void updateTree() override;

	// Abstract functions for print the tree
	virtual void print() const override {};
	virtual void printNice() const override {};
};