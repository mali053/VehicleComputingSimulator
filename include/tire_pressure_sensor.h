#ifndef __TIRE_PRESSURE_SENSOR_H__
#define __TIRE_PRESSURE_SENSOR_H__

#include "sensor.h"

// Example for sensor - tire pressure sensor
class TirePressureSensor : public Sensor {
public:
    TirePressureSensor(int id) : Sensor(id) {}

    // Executes a specified action for the tire pressure sensor.
    virtual void doAction(string action) override;
};

#endif  // __TIRE_PRESSURE_SENSOR_H__