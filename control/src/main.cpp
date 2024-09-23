#include <iostream>
#include <vector>
#include "sensor.h"
#include "input.h"
#include "full_condition.h"
#include "global_properties.h"
// #include "../parser_json/src/packet_parser.h"
using namespace std;

int main()
{
    cout << "Starting Main..." << endl;
    GlobalProperties &instanceGP = GlobalProperties::getInstance();
    // Build the conditions from the bson file
    Input::s_buildConditions();

    cout << "Starting Communication\n..." << endl;
    // Starting communication with the server
    instanceGP.comm->startConnection();

    // Running in a time loop to receive messages and handle them
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

#pragma region previous
    // vector<pair<int, string>> vec1 = {{5, "slow down"}, {8, "ccc"}};
    // vector<pair<int, string>> vec2 = {{8, "good!!!"}};

    // // Build the condition tree
    // FullCondition cond(
    //     "|([5]&(|(=(code,500),<(status,\"high\")),=(msg,\"aaa\")),[5]&(|(=("
    //     "code,500),<(status,\"high\")),>(msg,\"aaa\")))",
    //     vec1);
    // instanceGP.conditions.insert({cond.id, &cond});
    // FullCondition c2("[5]|(=(code,500),<(status,\"high\"))", vec2);
    // instanceGP.conditions.insert({c2.id, &c2});

    // // --Test updates in the sensors--

    // // Vectors for fields, values, and IDs to be updated
    // vector<string> fields = {"code", "code", "status", "msg", "status"};
    // vector<string> values = {"800", "500", "\"high\"", "\"aaa\"", "600"};
    // vector<int> ids = {8, 5, 5, 5, 8};

    // // Number of updates to process
    // size_t numUpdates = ids.size();

    // // Loop through each update
    // for (int i = 0; i < numUpdates; ++i) {
    //     int id = ids[i];

    //     // Find the sensor with the given ID
    //     Sensor *sensor = instanceGP.sensors[id];

    //     // If the sensor is found, change its field value and process the results
    //     if (sensor != nullptr) {
    //         sensor->updateTrueRoots(fields[i], values[i]);
    //         cout << "After update in sensor " << ids[i] << ": " << fields[i]
    //              << " " << values[i] << endl;
    //         for (int cId : instanceGP.trueConditions)
    //             instanceGP.conditions[cId]->activateActions();
    //     }
    // }

#pragma endregion

    return 0;
}