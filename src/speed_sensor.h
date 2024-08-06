#pragma once
#include "sensor.h"

// Examples for sensors - speed sensor and tire pressure sensor
class SpeedSensor: public Sensor
{
public:
	//Executes a specified action for the speedSensor.
	speedSensor(int id) : Sensor(id) { }

	virtual void doAction(string action);
};

class TirePressureSensor : public Sensor
{
public:
	TirePressureSensor(int id) : Sensor(id) { }

	virtual void doAction(string action) { cout << "I sensor " << id << " do " << action << endl; };
};
