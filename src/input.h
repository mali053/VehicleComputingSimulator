#pragma once
#include <fstream>
#include <map>
#include <vector>
#include <fstream>
#include "json.hpp"
#include "sensor.h"
using namespace std;
using json = nlohmann::json;

// An enum that contains all possible sensor types
enum SensorsTypes {
	Speed, TirePressure, Light, Unknown
};

static class Input
{
public:
	// Function that builds the sensors according to the json file
	static void s_buildSensors(vector<Sensor*>& sensors);

private:
	// Function that read the json file
    static json& s_readData();
	// Member that contains the json file data
    static json s_jsonToRead;

	// Function that take a string and return the corresponding sensor type
	static SensorsTypes s_convertStringToSensorsTypes(string type);
};