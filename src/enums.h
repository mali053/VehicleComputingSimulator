#pragma once
#include <string>
#include <unordered_map>
using namespace std;

enum OperatorTypes {
	o, // ||
	a, // &&
	b, // >
	l,  // <
	e,  // =
	ne, // !=
	be, // >=
	le,  // <=
	UnknownOperatorType
};

static OperatorTypes s_convertStringToOperatorTypes(string operatorType)
{
	unordered_map<string, OperatorTypes> map = {
		{"|", OperatorTypes::o},
		{"&", OperatorTypes::a},
		{">", OperatorTypes::b},
		{"<", OperatorTypes::l},
		{"=", OperatorTypes::e},
		{"!=", OperatorTypes::ne},
		{">=", OperatorTypes::be},
		{"<=", OperatorTypes::le}
	};

	auto it = map.find(operatorType);
	return ((it != map.end()) ? (it->second) : (OperatorTypes::UnknownOperatorType));
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