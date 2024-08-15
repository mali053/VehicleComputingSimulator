#include "input.h"

// Function that builds the sensors according to the json file
void Input::s_buildSensors(unordered_map<int, Sensor *> &sensors)
{
    Sensor *sensorPtr;
    SensorsTypes sensorType;

    // Go over the sensors in json and create parallel sensors
    for (map<string, string> sensor : Input::s_jsonToRead["Sensors"]) {
        sensorPtr = nullptr;
        sensorType = convertStringToSensorsTypes(sensor["name"]);

        // Create parallel sensors
        switch (sensorType) {
            case SensorsTypes::Speed: {
                sensorPtr = new SpeedSensor(stoi(sensor["id"]));
                break;
            }
            case SensorsTypes::TirePressure: {
                sensorPtr = new TirePressureSensor(stoi(sensor["id"]));
                break;
            }
            default:
                break;
        }

        // Add to the vector a pointer to the sensor
        if (sensorPtr != nullptr)
            sensors[sensorPtr->id] = sensorPtr;
    }
}

// Function that read the json file
json &Input::s_readData()
{
    // Read the json file
    std::ifstream f("json_to_read.json");

    // Check if the input is correct
    if (!f.is_open()) {
        std::cerr << "Failed to open json_to_read.json" << std::endl;
    }
    json *data = NULL;
    // Try parse to json type and return it
    try {
        data = new json(json::parse(f));
        return *data;
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
    catch (...) {
        cout << "My Unknown Exception" << endl;
    }

    return *data;
}

// Load the json file into s_jsonToRead
json Input::s_jsonToRead = s_readData();
