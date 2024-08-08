#include "test_helper.h"
void testCondition(string condition, int sensorId, string field, string value)
{
    FullCondition::s_counter = 0;
    FullCondition::s_existingConditions = {};
    GlobalProperties::resetInstance();

    GlobalProperties& instanceGP = GlobalProperties::getInstance();
    map<int, string> map = { {8, "the condition is true"} };
    FullCondition* cond = new FullCondition(condition, map);
    instanceGP.conditions[cond->id] = cond;
    Sensor* sensor = instanceGP.sensors[sensorId];
    sensor->updateTrueRoots(field, value);
    // cout << "After update in sensor " << sensorId << ": " << field << " " << value << endl;
    // for (int cId : g_instanceGP.trueConditions)
    //     sendToActions(g_instanceGP.conditions[cId]->actions);
    // cout << "testCondition" << endl;
}



// Fuction that activates all actions in the vector 
void sendToActions(map<int, string> actions) {
    GlobalProperties& g_instanceGP = GlobalProperties::getInstance();
	for (pair<int, string> action : actions) {
        Sensor* destinationSensor = g_instanceGP.sensors[action.first];
		destinationSensor->doAction(action.second);
	}
}