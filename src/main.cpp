#include <iostream>
#include <vector>
#include "sensor.h"
#include "input.h"
#include "full_condition.h"
#include "global_properties.h"
#include "test_helper.h"
using namespace std;

GlobalProperties &g_instanceGP = GlobalProperties::getInstance();

int main()
{
    map<int, string> map1 = {{5, "slow down"}, {8, "ccc"}};
    map<int, string> map2 = {{8, "good!!!"}};

    // Build the condition tree
    FullCondition cond(
        "|([5]&(|(=(code,500),<(status,\"high\")),=(msg,\"aaa\")),[5]&(|(=("
        "code,500),<(status,\"high\")),>(msg,\"aaa\")))",
        map1);
    g_instanceGP.conditions.insert({cond.id, &cond});
    FullCondition c2("[5]|(=(code,500),<(status,\"high\"))", map2);
    g_instanceGP.conditions.insert({c2.id, &c2});

    // --Test updates in the sensors--

    // Vectors for fields, values, and IDs to be updated
    vector<string> fields = {"code", "code", "status", "msg"};
    vector<string> values = {"800", "500", "\"high\"", "\"aaa\""};
    vector<int> ids = {8, 5, 5, 5};

    // Number of updates to process
    size_t numUpdates = ids.size();

    // Loop through each update
    for (int i = 0; i < numUpdates; ++i) {
        int id = ids[i];

        // Find the sensor with the given ID
        Sensor *sensor = g_instanceGP.sensors[id];

        // If the sensor is found, change its field value and process the results
        if (sensor != nullptr) {
            sensor->updateTrueRoots(fields[i], values[i]);
            cout << "After update in sensor " << ids[i] << ": " << fields[i]
                 << " " << values[i] << endl;
            for (int cId : g_instanceGP.trueConditions)
                g_instanceGP.conditions[cId]->activateActions();
        }
    }

    return 0;
}