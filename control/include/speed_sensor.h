#ifndef __SPEED_SENSOR_H__
#define __SPEED_SENSOR_H__

#include "sensor.h"

// Example for sensor - speed sensor
class SpeedSensor : public Sensor {
public:
    SpeedSensor(int id) : Sensor(id) {}

    // Executes a specified action for the speed sensor.
    virtual void doAction(string action);
};

#endif  // _SPEED_SENSOR_H_