#pragma once
#include "condition.h"

class Node : public Condition {
public:
	// countTrueConditions: contains the of children whose status is true
	int countTrueConditions;

	// C-tor that initializes the countTrueConditions into 0
	Node() : countTrueConditions(0) {};

	// Function that updates the status of all nodes that should change
	virtual void updateTree() = 0;
};