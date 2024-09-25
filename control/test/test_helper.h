#ifndef TEST_HELPER_H
#define TEST_HELPER_H

#include <string>
#include <iostream>
#include <vector>
#include "sensor.h"
#include "input.h"
#include "full_condition.h"
#include "global_properties.h"

// Tests a condition by resetting global state and updating sensor values
void testCondition(string condition, int sensorId, string field, FieldValue value,
                   FieldType type);

#endif  // TEST_HELPER_H