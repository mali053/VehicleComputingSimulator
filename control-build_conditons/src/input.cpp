# include "input.h"

Input::Input()
{
    // Read the json file
    ifstream f(fileName);

    // Check if the input is correct
    if (!f.is_open())
        cerr << "Failed to open " << fileName << endl;

    json *data = NULL;

    // Try parse to json type
    try {
        data = new json(json::parse(f));
    }
    catch (exception e) {
        cout << e.what() << endl;
        return;
    }
    catch (...) {
        cout << "My Unknown Exception" << endl;
        return;
    }
    sensors = *data;
    fillSensorsFields();
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

// Static member to hold the single instance of `Output`
unique_ptr<Input> Input::instance = NULL;

// Populate sensor fields list based on JSON file paths
void Input::fillSensorsFields()
{
    for (auto& [sensorId, sensorData] : sensors.items()) {        // sensor["fields"] = getFieldsOfSensor(sensor["pathToJson"]);
        sensorData["fields"] = getFieldsOfSensor("sensors_data/" + sensorData["name"].get<string>() + ".json");
        cout << sensorId << " - " << sensorData["name"] << " : " << sensorData["pathToJson"] << endl;
    }
}

// Read fields from a sensor's JSON file
json Input::getFieldsOfSensor(string psthToSensorJson)
{
    // Read the json file
    ifstream f(psthToSensorJson);

    // Check if the input is correct
    if (!f.is_open())
        cerr << "Failed to open " << psthToSensorJson << endl;

    json *data = NULL;

    // Try parse to json type
    try {
        data = new json(json::parse(f));
    }
    catch (exception e) {
        cout << e.what() << endl;
    }
    catch (...) {
        cout << "My Unknown Exception" << endl;
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