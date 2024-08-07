#pragma once
#include<iostream>
#include <vector>
#include <optional>
#include "node.h"
using namespace std;

// Abstract class that present an internal node in the tree
class OperatorNode : public Node {
public:
	// conditions: contains all the sub conditions that this condition consists of- the children of the node
	vector<Condition*> conditions;

	// Function that updates the status of all nodes that should change
	void updateTree() override;
	// Abstract helper function that update the status of the current status
	virtual void updateStatus() = 0;

};