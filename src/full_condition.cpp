#include "full_condition.h"

//example:
//|([5]|(=(status,"high"),<(code,500),=(msg,"aaa")),[8]=(code,700))

// Global pointer to the current sensor on which the condition is conditional
Sensor* currentSensor;

// Global map to keep track of existing conditions to avoid duplication
unordered_map<string, Condition*> FullCondition::s_existingConditions = {};

// Initialize a static variable to assign unique IDs to each FullCondition instance
int FullCondition::s_counter = 0;

// Handling sensor reference
void defineCurrentSensor(const string& condition, int& index)
{
	GlobalProperties& instanceGP = GlobalProperties::getInstance();

	int closeBracket = find(condition.begin() + index, condition.end(), ']') - condition.begin();
	string numStr = condition.substr(index + 1, closeBracket - index - 1);
	int id = stoi(numStr);
	index = closeBracket + 1;
	currentSensor = instanceGP.sensors[id];
}

// Recursively builds the condition tree from the condition string.
Condition* FullCondition::buildNode(const string& condition, int& index, map<int, int> bracketIndexes) {
	
	GlobalProperties& instanceGP = GlobalProperties::getInstance();

	if (condition.empty())
		throw "Condition string is empty!";

	// Handling sensor reference
	if (condition[index] == '[')
		defineCurrentSensor(condition, index);

	int openBracketIndex = find(condition.begin() + index, condition.end(), '(') - condition.begin();
	// Generates a key for the condition with the current sensor's ID (if exists)
	string key = (currentSensor ? to_string(currentSensor->id) : "") + condition.substr(index, bracketIndexes[openBracketIndex] - index + 1);
	// Check if the key already exists in the existingConditions map
	if (s_existingConditions.find(key) != s_existingConditions.end()) {
		index = bracketIndexes[openBracketIndex] + 1;
		if (condition[index] == ',')
			index++;
		return s_existingConditions.find(key)->second;
	}

	// | , & , ( = , < , > , >= , <= , != )
	// Creating a new node
	OperatorTypes operatorType = s_convertStringToOperatorTypes(condition.substr(index, openBracketIndex - index));
	Condition* conditionPtr = createCondition(operatorType);
	
	s_existingConditions[key] = conditionPtr;

	if (OperatorNode* operatorNode = dynamic_cast<OperatorNode*>(conditionPtr)) {
		index += 2;
		int count = 0;

		// Going over the internal conditions and creating children
		while (condition[index] != ')') {
			// Handling sensor reference
			if (condition[index] == '[')
				defineCurrentSensor(condition, index);

			// Handling same operator
			string temp = condition.substr(index, 1);
			while (s_convertStringToOperatorTypes(temp) == operatorType) {
				count++;
				index += 2;
				temp = condition.substr(index, 1);
			}

			operatorNode->conditions.push_back(buildNode(condition, index, bracketIndexes));
			operatorNode->conditions[operatorNode->conditions.size() - 1]->parents.push_back(operatorNode);
			
			while (condition[index] == ')' && count) {
				count--;
				index++;
			}
			if (condition[index] == ',')
				index++;
		}
	}
	else if (BasicCondition* basicCondition = dynamic_cast<BasicCondition*>(conditionPtr)) {
		// Fill the fields in BasicCondition
		basicCondition->operatorType = operatorType;
		int commaIndex = find(condition.begin() + index, condition.end(), ',') - condition.begin();
		string name = condition.substr(openBracketIndex + 1, commaIndex - openBracketIndex - 1);
		int closeBracket = bracketIndexes[openBracketIndex];
		basicCondition->value = condition.substr(commaIndex + 1, closeBracket - commaIndex - 1);
		// Add the sensor reference to this leaf
		currentSensor->fields[name].second.push_back(basicCondition);
	}

	index = bracketIndexes[openBracketIndex] + 1;
	if (condition[index] == ',')
		index++;

	return conditionPtr;
}

// Maps the positions of opening bracket indexes to their corresponding closing bracket indexes.
map<int, int> findBrackets(string condition)
{
	map<int, int> mapIndexes;
	stack<int> stackIndexes;
	for (int i = 0; i < condition.size(); i++) {
		if (condition[i] == '(') {
			stackIndexes.push(i);
		}
		else if (condition[i] == ')') {
			mapIndexes[stackIndexes.top()] = i;
			stackIndexes.pop();
		}
	}

	return mapIndexes;
}

// Constructor: Builds the condition tree.
FullCondition::FullCondition(string condition, map<int, string>& actions) : actions(actions)
{
	id = FullCondition::s_counter++;
	map<int, int> bracketsIndexes = findBrackets(condition);
	int index = 0;
	Condition* firstCondition = this->buildNode(condition, index, bracketsIndexes);
	root = new Root(this->id, firstCondition);
	firstCondition->parents.push_back(root);
	currentSensor = nullptr;
}