#ifndef __NODE_H__
#define __NODE_H__


#include "condition.h"

class Node : public Condition {
public:
	// countTrueConditions: contains the of children whose status is true
	int countTrueConditions;

	virtual ~Node() = default;
	// C-tor that initializes the countTrueConditions into 0
	Node() : countTrueConditions(0) {};

	// Function that updates the status of all nodes that should change
	virtual void updateTree() = 0;
};
#endif // _NODE_H_
