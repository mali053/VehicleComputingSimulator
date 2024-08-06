#pragma once
#include "sensor.h"

class speedSensor: public Sensor
{
public:
	speedSensor(int id) : Sensor(id) { }

	virtual void doAction(string action);
};

class TirePressureSensor : public Sensor
{
public:
	TirePressureSensor(int id) : Sensor(id) { }

	virtual void doAction(string action) { cout << "I sensor " << id << " do " << action << endl; };
};
