#include "test_helper.h"
using namespace std;

// Tests a condition by resetting global state and updating sensor values
void testCondition(string condition, int sensorId, string field, FieldValue value,
                   FieldType type)
{
    // Resets counters and existing conditions, recreates the `GlobalProperties` instance
    FullCondition::s_counter = 0;
    FullCondition::s_existingConditions = {};
    GlobalProperties::resetInstance();

    GlobalProperties &instanceGP = GlobalProperties::getInstance();

    // Creates a `FullCondition` object with the given parameters
    vector<pair<int, string>> vec = {{8, "the condition is true"}};
    FullCondition *cond = new FullCondition(condition, vec);
    instanceGP.conditions[cond->id] = cond;

    // Updates a specified sensor with the provided field and value
    Sensor *sensor = instanceGP.sensors[sensorId];
    sensor->updateTrueRoots(field, value, type);
}