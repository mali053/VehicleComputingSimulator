#pragma once
#include <string>
#include <unordered_map>
using namespace std;

enum operatorTypes {
	b, // >
	l,  // <
	e,  // =
	ne, // !=
	be, // >=
	le,  // <=
	UnknownOperatorType
};

static operatorTypes s_convertStringToOperatorTypes(string operatorType)
{
	unordered_map<string, operatorTypes> map = {
		{">", operatorTypes::b},
		{"<", operatorTypes::l},
		{"=", operatorTypes::e},
		{"!=", operatorTypes::ne},
		{">=", operatorTypes::be},
		{"<=", operatorTypes::le}
	};

	auto it = map.find(operatorType);
	return ((it != map.end()) ? (it->second) : (operatorTypes::UnknownOperatorType));
}

// An enum that contains all possible sensor types
enum SensorsTypes {
	Speed, TirePressure, Light, UnknownSensorType
};

// Function that take a string and return the corresponding sensor type
static SensorsTypes s_convertStringToSensorsTypes(string type)
{
	// Define a map to convert from string to sensor type
	static unordered_map<string, SensorsTypes> s_map = {
		{"Speed", SensorsTypes::Speed},
		{"Tire Pressure", SensorsTypes::TirePressure},
		{"Light", SensorsTypes::Light}
	};

	// Find the right type in the map and return it
	auto it = s_map.find(type);
	return ((it == s_map.end()) ? (SensorsTypes::UnknownSensorType) : (it->second));
}