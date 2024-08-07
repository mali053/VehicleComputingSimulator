#pragma once
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include "full_condition.h"
#include <input.h>
using namespace std;

class FullCondition;

class GlobalProperties {
private:
	static unique_ptr<GlobalProperties> instance;
	GlobalProperties();

public:
	static GlobalProperties& getInstance();
	unordered_map<int, Sensor*> sensors;
	unordered_map<int, FullCondition> conditions;
	unordered_set<int> trueConditions;
};
