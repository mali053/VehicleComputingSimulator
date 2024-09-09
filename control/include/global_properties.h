#ifndef __GLOBAL_PROPERTIES_H__
#define __GLOBAL_PROPERTIES_H__

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <memory>

#include "input.h"
#include "full_condition.h"

using namespace std;

class FullCondition;

// Singleton class managing global properties
class GlobalProperties {
private:
    // Single instance of the class
    static unique_ptr<GlobalProperties> instance;
    // Private constructor
    GlobalProperties();

public:
    // Gets the singleton instance
    static GlobalProperties &getInstance();
    // Resets the singleton instance
    static void resetInstance();

    // Map of sensors by ID
    unordered_map<int, Sensor *> sensors;
    // Map of conditions by ID
    unordered_map<int, FullCondition *> conditions;
    // Set of true condition IDs
    unordered_set<int> trueConditions;
};

#endif  // _GLOBAL_PROPERTIES_H_