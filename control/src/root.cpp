#include "root.h"

// Function that updates the status of all nodes that should change
void Root::updateTree()
{
    GlobalProperties &instanceGP = GlobalProperties::getInstance();

    // Update the status and insert or remove from trueConditions
    if (this->countTrueConditions) {
        instanceGP.trueConditions.insert(this->conditionId);
        this->status = true;
    }
    else {
        instanceGP.trueConditions.erase(this->conditionId);
        this->status = true;
    }
}