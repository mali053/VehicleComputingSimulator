#ifndef __INPUT_H__
#define __INPUT_H__

#include <iostream>
#include <bson/bson.h>
#include <map>
#include <vector>
#include <fstream>
#include <unordered_map>

#include "full_condition.h"

// Forward declaration instead of #include
class Sensor;

class Input {
public:
    // Function that builds the sensors according to the bjson file
    static void s_buildSensors(std::unordered_map<int, Sensor *> &sensors);
    // Function that builds the conditions according to the bson file
    static void s_buildConditions();

private:
    // Function that read the bson file
    static bson_t *s_readData();
    // Member that contains the bson file data
    static bson_t *document;
};

#endif  // _INPUT_H_