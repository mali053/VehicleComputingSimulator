# include "input.h"

using namespace std;

Input::Input()
{
    // Read the json file
    ifstream f(fileName);

    // Check if the input is correct
    if (!f.is_open())
        Output::controlLogger.logMessage(logger::LogLevel::ERROR, "Failed to open " + fileName);

    json *data = NULL;

    // Try parse to json type
    try {
        data = new json(json::parse(f));
    }
    catch (exception e) {
        Output::controlLogger.logMessage(logger::LogLevel::ERROR, e.what());
        return;
    }
    catch (...) {
        Output::controlLogger.logMessage(logger::LogLevel::ERROR, "My Unknown Exception");
        return;
    }
    sensors = *data;
    fillSensorsFields();

    Output::controlLogger.logMessage(logger::LogLevel::DEBUG, "Initialized Input object with sensors data.");
}

Input &Input::getInstance()
{
    // Creates the instance if it does not exist
    if (!Input::instance) {
        instance = unique_ptr<Input>(new Input());
    }
    // return Reference to the singleton `Input` instance
    return *instance;
}

void Input::setPathToSensors(std::string path)
{
    fileName = path;
    Output::controlLogger.logMessage(logger::LogLevel::INFO, "Updated file path to sensors.json: " + fileName);
    // Recreate the singleton instance
    instance.reset(new Input());
}

// Static member to hold the single instance of `Output`
unique_ptr<Input> Input::instance = NULL;

// Populate sensor fields list based on JSON file paths
void Input::fillSensorsFields()
{
    for (auto& [sensorId, sensorData] : sensors.items()) { 
        Output::controlLogger.logMessage(logger::LogLevel::DEBUG, sensorId + " - " + string(sensorData["name"]) + " : " + string(sensorData["pathToJson"]));
        sensorData["fields"] = getFieldsOfSensor(sensorData["pathToJson"]);
    }
}

// Read fields from a sensor's JSON file
json Input::getFieldsOfSensor(string psthToSensorJson)
{
    // Read the json file
    ifstream f(psthToSensorJson);

    // Check if the input is correct
    if (!f.is_open())
        Output::controlLogger.logMessage(logger::LogLevel::ERROR, "Failed to open " + psthToSensorJson);

    json *data = NULL;

    // Try parse to json type
    try {
        data = new json(json::parse(f));
    }
    catch (exception e) {
        Output::controlLogger.logMessage(logger::LogLevel::ERROR, e.what());
    }
    catch (...) {
        Output::controlLogger.logMessage(logger::LogLevel::ERROR, "My Unknown Exception");
    }

    // Read the fields and return them
    json fields = json::array();

    for (auto field : (*data)["fields"]) {
        if (field["type"] == "bit_field")
            for (auto subField : field["fields"]) {
                string subFieldName = subField["name"];
                json sensorData;
                sensorData[subFieldName] = subField["type"];
                fields.push_back(sensorData);
            }
        else {
            string fieldName = field["name"];
            json sensorData;
            sensorData[fieldName] = field["type"];
            fields.push_back(sensorData);
        }
    }
    json *array = new json(fields);
    return *array;
}