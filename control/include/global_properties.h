#ifndef __GLOBAL_PROPERTIES_H__
#define __GLOBAL_PROPERTIES_H__

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

#include <nlohmann/json.hpp>
#include "input.h"
#include "full_condition.h"

#include "../../communication/src/communication.h"

using json = nlohmann::json;

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

    // Communication
    uint32_t srcID = 1;
    // Creating the communication object with the callback function to process the data
    Communication *comm;
};

#endif  // _GLOBAL_PROPERTIES_H_