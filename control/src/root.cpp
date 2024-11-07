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
    
    GlobalProperties::controlLogger.logMessage(logger::LogLevel::INFO, "Root with id " + to_string(this->conditionId) + " changed into " + (this->status ? "true" : "false") + " state");
}