
#ifndef __SPEED_SENSOR_H__
#define __SPEED_SENSOR_H__


#include "sensor.h"

// Examples for sensors - speed sensor and tire pressure sensor
class SpeedSensor: public Sensor
{
public:
	//Executes a specified action for the speedSensor.
	SpeedSensor(int id) : Sensor(id) { }

	virtual void doAction(string action);
};

class TirePressureSensor : public Sensor
{
public:
	TirePressureSensor(int id) : Sensor(id) { }

	virtual void doAction(string action) { cout << "I sensor " << id << " do " << action << endl; };
};

#endif // _SPEED_SENSOR_H_
