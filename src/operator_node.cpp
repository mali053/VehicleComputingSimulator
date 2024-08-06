#include "operator_node.h"

optional<bool> OperatorNode::updateTree()
{
    bool prevStatus = this->status; 
    optional<bool> isRootTrue = nullopt, currentParent = nullopt;
    

    this->updateStatus();
    // Checking what was happening with the root was true and we got to it with truth
    if (prevStatus != this->status) {
        
        if (parents.size() == 0)
            return this->status;
        for (OperatorNode* parent : this->parents) {
            (this->status) ? parent->countTrueConditions++ : parent->countTrueConditions--;
            currentParent = parent->updateTree();
                if (currentParent != nullopt)
                    isRootTrue = currentParent;
        }
    }
	return isRootTrue;
}
