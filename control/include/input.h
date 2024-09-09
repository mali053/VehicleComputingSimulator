#ifndef __INPUT_H__
#define __INPUT_H__

#include <iostream>
#include <bson/bson.h>
#include <map>
#include <vector>
#include <fstream>
#include <unordered_map>

#include "sensor.h"
#include "speed_sensor.h"
#include "tire_pressure_sensor.h"
#include "full_condition.h"

using namespace std;

class Input {
public:
    // Function that builds the sensors according to the bjson file
    static void s_buildSensors(unordered_map<int, Sensor *> &sensors);
    // Function that builds the conditions according to the bson file
    static void s_buildConditions();

private:
    // Function that read the bson file
    static bson_t *s_readData();
    // Member that contains the bson file data
    static bson_t *document;
};

#endif  // _INPUT_H_