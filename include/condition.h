
#ifndef __CONDITION_H__
#define __CONDITION_H__


#include <vector>
#include <iostream>
using namespace std;

class Node;

// Abstract class that present a node in the tree
class Condition {
public:
	// status: contains what is the boolean value of this subtree
	bool status = false;
	// parents: contains pointers to all parents I'm their son- reduce same subtrees in the tree
	vector<Node*> parents;

	virtual ~Condition() = default;

	// Abstract functions for print the tree
	virtual void print() const = 0;
	virtual void printNice() const = 0;
};
#endif // _CONDITION_H_
