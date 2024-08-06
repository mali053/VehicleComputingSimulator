#pragma once
#include <vector>
#include <iostream>
using namespace std;

class OperatorNode;

// Abstract class that present a node in the tree
class Condition {
public:
	// status: contains what is the boolean value of this subtree
	bool status = false;
	// parents: contains pointers to all parents I'm their son- reduce same subtrees in the tree
	vector<OperatorNode*> parents;

	virtual ~Condition() {}

	// Abstract functions for print the tree
	virtual void print() const = 0;
	virtual void printNice() const = 0;
};