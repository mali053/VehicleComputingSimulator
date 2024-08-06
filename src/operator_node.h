#pragma once
#include<iostream>
#include <vector>
#include <optional>
#include "condition.h"
using namespace std;

// Abstract class that present an internal node in the tree
class OperatorNode : public Condition {
public:
	// conditions: contains all the sub conditions that this condition consists of- the children of the node
	vector<Condition*> conditions;
	// countTrueConditions: contains the of children whose status is true
	int countTrueConditions;

	// C-tor that initializes the countTrueConditions into 0
	OperatorNode() : countTrueConditions(0) {};

	// Function that updates the status of all nodes that should change
	optional<bool> updateTree();
	// Abstract helper function that update the status of the current status
	virtual void updateStatus() = 0;

};