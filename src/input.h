#pragma once
#include <fstream>
#include <map>
#include <vector>
#include <fstream>
#include <unordered_map>
#include "json.hpp"
#include "sensor.h"
#include "speed_sensor.h"
using namespace std;
using json = nlohmann::json;

static class Input
{
public:
	// Function that builds the sensors according to the json file
	static void s_buildSensors(unordered_map<int, Sensor*>& sensors);

private:
	// Function that read the json file
    static json& s_readData();
	// Member that contains the json file data
    static json s_jsonToRead;
};