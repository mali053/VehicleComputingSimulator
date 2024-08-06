#include "Sensor.h"


enum operatorTypes {
	b, // >
	l,  // <
	e,  // =
	ne, // !=
	be, // >=
	le  // <=
};


set<int> Sensor::changeValueOfField(string field, string value)
{
	set<int> result;
	this->fields[field].first = value;

	unordered_map<string, operatorTypes> map = {
		{">", operatorTypes::b},
		{"<", operatorTypes::l},
		{"=", operatorTypes::e},
		{"!=", operatorTypes::ne},
		{">=", operatorTypes::be},
		{"<=", operatorTypes::le}
	};

	for (BasicCondition* bc : this->fields[field].second) {

		bool flag = false, prevStatus = bc->status;
		string bcValue = bc->value;

		operatorTypes myOperator = map[bc->operatorType];

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

		bc->status = flag;

		optional<bool> currentParent = nullopt, isRootTrue = nullopt;
		
		if (flag != prevStatus) {
			
			if (bc->parents.size() == 0) {
				if (bc->status)
					result.insert(bc->conditionId);
			}
			else {
				for (OperatorNode* parent : bc->parents) {
					(bc->status) ? parent->countTrueConditions++ : parent->countTrueConditions--;
					currentParent = parent->updateTree();
					if (currentParent != nullopt)
						isRootTrue = currentParent;
				}
				
				if (isRootTrue)
					result.insert(bc->conditionId);
			}	
			
		}
	}
	return result;
}
