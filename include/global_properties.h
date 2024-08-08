
#ifndef __GLOBAL_PROPERTIES_H__
#define __GLOBAL_PROPERTIES_H__


#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include "input.h"
#include "full_condition.h"
using namespace std;

 class FullCondition;

class GlobalProperties {
private:
	static unique_ptr<GlobalProperties> instance;
	GlobalProperties();

public:
	static GlobalProperties& getInstance();
	static void resetInstance();

	unordered_map<int, Sensor*> sensors;
	unordered_map<int, FullCondition*> conditions;
	unordered_set<int> trueConditions;
};

#endif // _GLOBAL_PROPERTIES_H_
