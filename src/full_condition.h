#pragma once
#include <map>
#include <string>
#include <algorithm>
#include <iostream>
#include <stack>
#include <unordered_map>
#include "sensor.h"
#include "or_operator.h"
#include "and_operator.h"
#include "basic_condition.h"
using namespace std;

class FullCondition {
private:
	// Recursively builds the condition tree from the condition string.
	Condition* buildNode(const string& condition, int& index, vector<Sensor*>& sensors, map<int, int> bracketIndexes);
public:
	// Static counter to assign unique IDs to each FullCondition instance
	static int counter;
	// Unique ID for the FullCondition instance
	int id;
	// Root node of the condition tree
	Condition* root; 
	// Map of actions associated with this condition
	map<int, string> actions; 


	// Constructor: Initializes the FullCondition object, parses the condition string, and builds the condition tree.
	FullCondition(string condition, map<int, string>& actions, vector<Sensor*>& sensors);


};