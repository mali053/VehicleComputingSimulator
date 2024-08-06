#include "Sensor.h"


enum operatorTypes {
	b, // >
	l,  // <
	e,  // =
	ne, // !=
	be, // >=
	le  // <=
};

//
set<int> Sensor::updateStatusAndGetTrueRoots(string field, string value)
{
	// To store IDs of conditions that become true after the update
	set<int> result;

	// Update the field value in the sensor
	this->fields[field].first = value;

	unordered_map<string, operatorTypes> map = {
		{">", operatorTypes::b},
		{"<", operatorTypes::l},
		{"=", operatorTypes::e},
		{"!=", operatorTypes::ne},
		{">=", operatorTypes::be},
		{"<=", operatorTypes::le}
	};

	// Evaluate each condition related to the field

	for (BasicCondition* bc : this->fields[field].second) {

		bool flag = false, prevStatus = bc->status;
		string bcValue = bc->value;

		operatorTypes myOperator = map[bc->operatorType];

		// Set the new status based on the operator and the value
		switch (myOperator) {
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

		optional<bool> currentParent = nullopt, isRootTrue = nullopt;

		// If the condition's status has changed
		if (flag != prevStatus) {
			// If no parents, add to result if condition is true
			if (bc->parents.size() == 0) {
				if (bc->status)
					result.insert(bc->conditionId);
			}
			else {
				// Update parent conditions and check if the root condition is true
				for (OperatorNode* parent : bc->parents) {
					(bc->status) ? parent->countTrueConditions++ : parent->countTrueConditions--;
					currentParent = parent->updateTree();
					if (currentParent != nullopt)
						isRootTrue = currentParent;
				}
				//Add the id of the full condition to the result if the root condition is true
				if (isRootTrue)
					result.insert(bc->conditionId);
			}

		}
	}
	return result;
}
