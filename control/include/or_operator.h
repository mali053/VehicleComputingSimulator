#ifndef __OR_OPERATOR_H__
#define __OR_OPERATOR_H__

#include "operator_node.h"

class OrOperator : public OperatorNode {
public:
    //Updates the status according to the type of operator
    void updateStatus() override;
};

#endif  // _OR_OPERATOR_H_