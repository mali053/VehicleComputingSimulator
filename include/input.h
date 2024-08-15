#ifndef __INPUT_H__
#define __INPUT_H__

#include <fstream>
#include <map>
#include <vector>
#include <fstream>
#include <unordered_map>
#include "../include/json.hpp"
#include "sensor.h"
#include "speed_sensor.h"
#include "tire_pressure_sensor.h"
using namespace std;
using json = nlohmann::json;

class Input {
public:
    // Function that builds the sensors according to the json file
    static void s_buildSensors(unordered_map<int, Sensor *> &sensors);

private:
    // Function that read the json file
    static json &s_readData();
    // Member that contains the json file data
    static json s_jsonToRead;
};

#endif  // _INPUT_H_