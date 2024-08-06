#include "full_condition.h"
//|([5]|(=(status,"high"),<(code,500),=(msg,"aaa")),[8]=(code,700))
Sensor* currentSensor;
unordered_map<string, Condition*> existingConditions;
int FullCondition::counter = 0;

Condition* FullCondition::buildNode(const string& condition, int& index, vector<Sensor*>& sensors, map<int, int> bracketIndexes) {
	if (condition.empty())
		throw "!!!";
	if (condition[index] == '[') {
		int closeBracket = find(condition.begin() + index, condition.end(), ']') - condition.begin();
		string numStr = condition.substr(index + 1, closeBracket - index - 1);
		int id = stoi(numStr);
		index = closeBracket + 1;
		currentSensor = *find_if(sensors.begin(), sensors.end(), [id](Sensor* s) { return s->id == id; });
	}
	int openBracketIndex = find(condition.begin() + index, condition.end(), '(') - condition.begin();
	string key = (currentSensor ? to_string(currentSensor->id) : "") + condition.substr(index, bracketIndexes[openBracketIndex] - index + 1);
	if (existingConditions.find(key) != existingConditions.end()) {
		index = bracketIndexes[openBracketIndex] + 1;//find(condition.begin() + index, condition.end(), ')') - condition.begin() + 1;
		if (condition[index] == ',')
			index++;
		return existingConditions.find(key)->second;
	}

	// | , & , ( = , < , > , >= , <= , != )
	Condition* conditionPtr;
	if (condition[index] == '|')
		conditionPtr = new OrOperator;
	else if (condition[index] == '&')
		conditionPtr = new AndOperator;
	else
		conditionPtr = new BasicCondition(this->id);
	existingConditions[key] = conditionPtr;
	if (OperatorNode* operatorNode = dynamic_cast<OperatorNode*>(conditionPtr)) {
		index += 2;
		//Going over the internal conditions and creating sons
		while (condition[index] != ')') {
			operatorNode->conditions.push_back(buildNode(condition, index, sensors, bracketIndexes));
			operatorNode->conditions[operatorNode->conditions.size() - 1]->parents.push_back(operatorNode);
		}
	}
	else if (BasicCondition* basicCondition = dynamic_cast<BasicCondition*>(conditionPtr)) {
		//fill the field in BasicCondition

		basicCondition->operatorType = condition.substr(index, openBracketIndex - index);
		int commaIndex = find(condition.begin() + index, condition.end(), ',') - condition.begin();
		string name = condition.substr(openBracketIndex + 1, commaIndex - openBracketIndex - 1);
		int closeBracket = bracketIndexes[openBracketIndex];
		basicCondition->value = condition.substr(commaIndex + 1, closeBracket - commaIndex - 1);
		//add the sensor reference to this leaf
		currentSensor->fields[name].second.push_back(basicCondition);
	}
	index = bracketIndexes[openBracketIndex] + 1;//find(condition.begin() + index, condition.end(), ')') - condition.begin() + 1;
	if (condition[index] == ',')
		index++;
	return conditionPtr;
}
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
FullCondition::FullCondition(string condition, map<int, string>& actions, vector<Sensor*>& sensors) : actions(actions)
{
	id = FullCondition::counter++;
	map<int, int> bracketsIndexes = findBrackets(condition);
	int index = 0;
	root = this->buildNode(condition, index, sensors, bracketsIndexes);
	existingConditions.clear();
}









