#include "input.h"
// Function that builds the sensors according to the BSON file
void Input::s_buildSensors(unordered_map<int, Sensor *> &sensors)
{
    // Get "Sensors" array from the BSON document.
    // If the "Sensors" array is not found or it is not a valid array, log an error and return.
    bson_iter_t iter;
    if (!bson_iter_init_find(&iter, document, "Sensors") && BSON_ITER_HOLDS_ARRAY(&iter)) {
        cerr << "Failed to find 'Sensors' array or it is not an array!" << endl;
        return;
    }

    // Extract the raw BSON data for the "Sensors" array.
    const uint8_t *sensorsData;
    uint32_t sensorsLength;
    bson_iter_array(&iter, &sensorsLength, &sensorsData);

    // Create a BSON object from the extracted array data.
    bson_t *bsonSensors = bson_new_from_data(sensorsData, sensorsLength);

    // Initialize an iterator to iterate over the "Sensors" array.
    bson_iter_t arrayIter;
    if (!bson_iter_init(&arrayIter, bsonSensors)) {
        // If initialization fails, clean up and return.
        bson_destroy(bsonSensors);
        return;
    }

    // Iterate through each document in the "Sensors" array.
    while (bson_iter_next(&arrayIter)) {
        if (!BSON_ITER_HOLDS_DOCUMENT(&arrayIter))
            break;  // If the current element is not a document, break the loop.

        // Recurse into the document.
        bson_iter_t docIter;
        bson_iter_recurse(&arrayIter, &docIter);

        string name;  // Sensor name/type.
        int id;       // Sensor ID.
        
        // Iterate over the keys in the current sensor document.
        while (bson_iter_next(&docIter)) {
            const char *key = bson_iter_key(&docIter);  // Get the current key.

            if (strcmp(key, "id") == 0)
                id = bson_iter_int32(&docIter);  // Extract the sensor ID.
            else if (strcmp(key, "name") == 0)
                name = bson_iter_utf8(&docIter, NULL);  // Extract the sensor name/type.
        }

        // Convert the string to a sensor type enum.
        SensorsTypes sensorType = convertStringToSensorsTypes(name);
        Sensor *sensorPtr = nullptr;  // Pointer to the sensor object.
        
        // Create the appropriate sensor object based on the sensor type.
        switch (sensorType) {
            case SensorsTypes::Speed: {
                sensorPtr = new SpeedSensor(id);  // Create a SpeedSensor.
                break;
            }
            case SensorsTypes::TirePressure: {
                sensorPtr = new TirePressureSensor(id);  // Create a TirePressureSensor.
                break;
            }
            default: {
                sensorPtr = new Sensor(id);  // Create a generic Sensor.
                break;
            }
        }

        // Add the sensor pointer to the map if it was successfully created.
        if (sensorPtr != nullptr)
            sensors[sensorPtr->id] = sensorPtr;
    }

    // Clean up by destroying the BSON object.
    bson_destroy(bsonSensors);
}

// Function that builds the conditions according to the BSON file.
void Input::s_buildConditions()
{
    // Get a reference to the GlobalProperties singleton instance.
    GlobalProperties &instanceGP = GlobalProperties::getInstance();

    // Get "Conditions" array from the BSON document.
    // If the "Conditions" array is not found or it is not valid, log an error and return.
    bson_iter_t iter;
    if (!bson_iter_init_find(&iter, document, "Conditions") || !BSON_ITER_HOLDS_ARRAY(&iter)) {
        cerr << "Failed to find 'Conditions' array or it is not an array!" << endl;
        return;
    }

    // Extract the raw BSON data for the "Conditions" array.
    const uint8_t *conditionsData;
    uint32_t conditionsLength;
    bson_iter_array(&iter, &conditionsLength, &conditionsData);

    // Create a BSON object from the extracted array data.
    bson_t *bsonConditions = bson_new_from_data(conditionsData, conditionsLength);

    // Initialize an iterator to iterate over the "Conditions" array.
    bson_iter_t arrayIter;
    if (!bson_iter_init(&arrayIter, bsonConditions)) {
        // If initialization fails, clean up and return.
        bson_destroy(bsonConditions);
        return;
    }

    // Iterate through each document in the "Conditions" array.
    while (bson_iter_next(&arrayIter)) {
        if (!BSON_ITER_HOLDS_DOCUMENT(&arrayIter))
            break;  // If the current element is not a document, break the loop.

        // Recurse into the condition document.
        bson_iter_t docIter;
        bson_iter_recurse(&arrayIter, &docIter);

        string conditionStr = "";  // The "if" condition string.
        vector<pair<int, string>> actions = {{5, "nothing"}};  // Default action with ID 5.

        // Iterate over the keys in the current condition document.
        while (bson_iter_next(&docIter)) {
            const char *key = bson_iter_key(&docIter);  // Get the current key.

            if (strcmp(key, "if") == 0) {
                // Extract the "if" condition string.
                conditionStr = bson_iter_utf8(&docIter, NULL);
            }
            else if (strcmp(key, "send") == 0) {
                // Process the "send" array which contains action messages.
                if (!BSON_ITER_HOLDS_ARRAY(&docIter)) {
                    cerr << "'send' is not an array!" << endl;
                    continue;  // Skip to the next key if "send" is not an array.
                }

                // Extract the raw BSON data for the "send" array.
                const uint8_t *sendData;
                uint32_t sendLength;
                bson_iter_array(&docIter, &sendLength, &sendData);

                // Create a BSON object from the extracted "send" array data.
                bson_t *bsonSend = bson_new_from_data(sendData, sendLength);

                // Initialize an iterator to iterate over the "send" array.
                bson_iter_t arraySendIter;
                if (!bson_iter_init(&arraySendIter, bsonSend)) {
                    // If initialization fails, clean up and continue.
                    bson_destroy(bsonSend);
                    continue;
                }

                // Iterate through each document in the "send" array.
                while (bson_iter_next(&arraySendIter)) {
                    if (!BSON_ITER_HOLDS_DOCUMENT(&arraySendIter))
                        break;  // If the current element is not a document, break the loop.

                    // Recurse into the action document.
                    bson_iter_t docSendIter;
                    bson_iter_recurse(&arraySendIter, &docSendIter);

                    int id = -1;  // Action ID.
                    string message = "";  // Action message.
                    
                    // Iterate over the keys in the current action document.
                    while (bson_iter_next(&docSendIter)) {
                        const char *keySend = bson_iter_key(&docSendIter);  // Get the current key.

                        if (strcmp(keySend, "id") == 0)
                            id = bson_iter_int32(&docSendIter);  // Extract the action ID.
                        else if (strcmp(keySend, "message") == 0)
                            message = bson_iter_utf8(&docSendIter, NULL);  // Extract the action message.
                    }

                    // If a valid ID was found, add the action to the list.
                    if (id != -1)
                        actions.push_back({ id, message });
                }

                // Clean up the BSON object for the "send" array.
                bson_destroy(bsonSend);
            }
        }

        // Create a new FullCondition object with the condition string and actions.
        FullCondition *cond = new FullCondition(conditionStr, actions);
        
        // Add the condition to the global conditions map using its ID as the key.
        instanceGP.conditions[cond->id] = cond;
        
        // Clear the actions vector for the next iteration.
        actions.clear();
    }

    // Clean up by destroying the BSON object.
    bson_destroy(bsonConditions);
}

// Function that read the bson file
bson_t *Input::s_readData()
{
    string fileName = "my_bson.bson";
    ifstream file(fileName, ios::binary);
    if (file.is_open()) {
        // Get the file size
        file.seekg(0, ios::end);
        streamsize size = file.tellg();
        file.seekg(0, ios::beg);

        // Read the file into a buffer
        vector<char> buffer(size);
        if (file.read(buffer.data(), size)) {
            // Process BSON data
            bson_t *bson = bson_new_from_data(
                reinterpret_cast<const uint8_t *>(buffer.data()), size);

            // Output the BSON document as JSON for debugging (before destroying BSON)
            char *str = bson_as_canonical_extended_json(bson, NULL);
            if (str != NULL) {
                // cout << "BSON Document: " << str << endl;
                bson_free(str);
                file.close();
                return bson;
            }
            else {
                cerr << "Error converting BSON document to JSON!" << endl;
            }
        }
        else {
            cerr << "Error reading data from file: " << fileName << endl;
        }
        file.close();
    }
    else {
        cerr << "Failed to open file for reading: " << fileName << endl;
    }
    return nullptr;
}

// Load the bson file into document
bson_t *Input::document = s_readData();