#include <iostream>
#include <vector>
#include "sensor.h"
#include "input.h"
#include "full_condition.h"
using namespace std;

vector<Sensor*> g_sensors;

// Fuction that activates all actions in the vector 
void sendToActions(map<int, string> actions) {
	for (pair<int, string> action : actions) {
		Sensor* destinationSensor = *find_if(g_sensors.begin(), g_sensors.end(), [action](Sensor* s) {return s->id == action.first; });
		destinationSensor->doAction(action.second);
	}
}

int main()
{
	// Build the sensors according the json file
	Input::s_buildSensors(g_sensors);


	map<int, string> map = { {5, "slow down"}, {8, "ccc"} };

#pragma region Conditions examples

	//FullCondition* cond = new FullCondition("[5]=(code,500)", map, g_sensors);
	
	//FullCondition* cond = new FullCondition("|([5]&(|(=(code,500),<(status,\"high\")),=(msg,\"aaa\")),[5]&(|(=(code,500),<(status,\"high\")),>(msg,\"aaa\")))", map, g_sensors);
	
	//&([5]&(=(status,\"high\"),=(code,500),=(msg,\"aaa\")),[8]=(code,800))

#pragma endregion
	
	// Build the condition tree
	FullCondition* cond = new FullCondition("|([5]&(|(=(code,500),<(status,\"high\")),=(msg,\"aaa\")),[5]&(|(=(code,500),<(status,\"high\")),>(msg,\"aaa\")))", map, g_sensors);


    // --Test updates in the sensors--

    // Vectors for fields, values, and IDs to be updated
    vector<string> fields = { "code", "code", "status", "msg" };
    vector<string> values = { "800", "500", "\"high\"", "\"aaa\"" };
    vector<int> ids = { 8, 5, 5, 5 };

    // Number of updates to process
    size_t numUpdates = ids.size();

    // Loop through each update
    for (int i = 0; i < numUpdates; ++i) {
        int id = ids[i];

        // Find the sensor with the given ID
        auto it = find_if(g_sensors.begin(), g_sensors.end(),
            [id](Sensor* s) { return s->id == id; });

        // If the sensor is found, change its field value and process the results
        if (it != g_sensors.end()) {
            set<int> result = (*it)->updateStatusAndGetTrueRoots(fields[i], values[i]);
            cout << "After update in sensor " << ids[i] << ": " << fields[i] << " " << values[i] << endl;
            for (int cId : result)
                sendToActions(cond[cId].actions);
        }
    }
	
	return 0;
}