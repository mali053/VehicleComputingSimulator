
#ifndef __AND_OPERATOR_H__
#define __AND_OPERATOR_H__


#include "operator_node.h"

class AndOperator : public OperatorNode {
public:
	//Updates the status according to the type of operator
	void updateStatus() override;

	// Functions for print the tree
	void print() const override;
	void printNice() const override;
};
#endif // _AND_OPERATOR_H_
