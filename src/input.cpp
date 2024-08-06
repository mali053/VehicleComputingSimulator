#include "input.h"
#include "sensor.h"
#include "speedSensor.h"

// Function that builds the sensors according to the json file
void Input::s_buildSensors(vector<Sensor*>& sensors)
{
	Sensor* sensorPtr;
	SensorsTypes sensorType;

	// Go over the sensors in json and create parallel sensors
	for (map<string, string> sensor : Input::s_jsonToRead["Sensors"]) {

		sensorPtr = nullptr;
		sensorType = s_convertStringToSensorsTypes(sensor["name"]);

		// Create parallel sensors
		switch (sensorType) {
		case SensorsTypes::Speed: {
			sensorPtr = new speedSensor(stoi(sensor["id"]));
			break;
		}
		case SensorsTypes::TirePressure: {
			sensorPtr = new TirePressureSensor(stoi(sensor["id"]));
			break;
		}
		default:
			break;
		}

		// Add to the vector a pointer to the sensor
		if (sensorPtr != nullptr)
			sensors.push_back(sensorPtr);
	}
}


// Function that read the json file
json& Input::s_readData()
{
	// Read the json file
	std::ifstream f("C:\\Users\\משתמש\\Desktop\\תכנות\\בוטקאמפ\\Project\\CMakeProject\\json_to_read.json");

	// Check if the input is correct
	if (!f.is_open()) {
		std::cerr << "Failed to open json_to_read.json" << std::endl;
	}

	// Try parse to json type and return it
	try {
		json* data = new json(json::parse(f));
		return *data;
	}
	catch (exception e) {
		cout << e.what() << endl;
	}
	catch (...) {
		cout << "My Unknown Exception" << endl;
	}

}


// Function that take a string and return the corresponding sensor type
SensorsTypes Input::s_convertStringToSensorsTypes(string type)
{
	// Define a map to convert from string to sensor type
	static unordered_map<string, SensorsTypes> s_map = {
		{"Speed", SensorsTypes::Speed},
		{"Tire Pressure", SensorsTypes::TirePressure},
		{"Light", SensorsTypes::Light}
	};

	// Find the right type in the map and return it
	auto it = s_map.find(type);
	return ((it == s_map.end()) ? (SensorsTypes::Unknown) : (it->second));
}

// Load the json file into s_jsonToRead
json Input::s_jsonToRead = s_readData();
