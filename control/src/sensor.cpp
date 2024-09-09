#include "sensor.h"

//Updates the condition status according to the received field and returns the  list of the full conditions whose root is true
void Sensor::updateTrueRoots(string field, string value)
{
    // To store IDs of conditions that become true after the update
    set<int> result;

    // Update the field value in the sensor
    this->fields[field].first = value;

    // Evaluate each condition related to the field

    for (BasicCondition *bc : this->fields[field].second) {
        bool flag = false, prevStatus = bc->status;
        string bcValue = bc->value;

        // Set the new status based on the operator and the value
        switch (bc->operatorType) {
            case b: {
                flag = value > bcValue;
                break;
            }
            case l: {
                flag = value < bcValue;
                break;
            }
            case e: {
                flag = value == bcValue;
                break;
            }
            case ne: {
                flag = value != bcValue;
                break;
            }
            case be: {
                flag = value >= bcValue;
                break;
            }
            case le: {
                flag = value <= bcValue;
                break;
            }
            default:
                break;
        }

        // Update the condition's status
        bc->status = flag;

        // If the condition's status has changed
        if (flag != prevStatus) {
            // Update parent conditions and check if the root condition is true
            for (Node *parent : bc->parents) {
                (bc->status) ? parent->countTrueConditions++
                             : parent->countTrueConditions--;
                parent->updateTree();
            }
        }
    }
}