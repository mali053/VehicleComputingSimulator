#ifndef TEST_HELPER_H
#define TEST_HELPER_H

#include <string>
#include <iostream>
#include <vector>
#include "sensor.h"
#include "input.h"
#include "full_condition.h"
#include "global_properties.h"
using namespace std;
// Declare the function
void testCondition(string condition, int sensorId, string field, string value);
void sendToActions(map<int, string> actions);

#endif  // TEST_HELPER_H