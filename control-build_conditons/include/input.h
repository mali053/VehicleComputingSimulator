#ifndef INPUT
#define INPUT

#include <QString>

#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <map>

#include "output.h"

using json = nlohmann::json;

// Singleton class which import from sensors.json
class Input {
private:
    // Private constructor
    Input();
    // Single instance of the class
    static std::unique_ptr<Input> instance;

    // Name of the file to which the information will be saved
    std::string fileName = "../sensors.json";

    // Functions for initialize the sensors fields
    void fillSensorsFields();
    json getFieldsOfSensor(std::string psthToSensorJson);

public:
    json sensors;

    // Gets the singleton instance
    static Input &getInstance();

    void setPathToSensors(std::string path);
};

#endif  // INPUT