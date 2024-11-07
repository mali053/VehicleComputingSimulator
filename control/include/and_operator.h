
#ifndef __AND_OPERATOR_H__
#define __AND_OPERATOR_H__

#include "operator_node.h"

class AndOperator : public OperatorNode {
public:
    //Updates the status according to the type of operator
    void updateStatus() override;
};

#endif  // _AND_OPERATOR_H_
