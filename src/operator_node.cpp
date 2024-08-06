#include "operator_node.h"

// Function that updates the status of all nodes that should change
optional<bool> OperatorNode::updateTree()
{
    bool prevStatus = this->status; 
    optional<bool> isRootTrue = nullopt, currentParent = nullopt; 

    // Update the status of the current node
    this->updateStatus();

    // TODO: Check what would happen if the root was true and we got to it with true

    // Check if the status changed
    if (prevStatus != this->status) {
        
        // If it is the root - return the status
        if (parents.size() == 0)
            return this->status;

        // Else - go over all the parents and update them
        for (OperatorNode* parent : this->parents) {
            (this->status) ? parent->countTrueConditions++ : parent->countTrueConditions--;
            currentParent = parent->updateTree();
            // If we got the root - update the isRootTrue varible
            if (currentParent != nullopt)
                isRootTrue = currentParent;
        }

    }

	return isRootTrue;
}
