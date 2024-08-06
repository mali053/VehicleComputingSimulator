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
public:
	static int counter;
	int id;
	Condition* root;
	map<int, string> actions;

	FullCondition(string condition, map<int, string>& actions, vector<Sensor*>& sensors);

private:
	Condition* buildNode(const string& condition, int &index, vector<Sensor*>& sensors, map<int, int> bracketIndexes);
};