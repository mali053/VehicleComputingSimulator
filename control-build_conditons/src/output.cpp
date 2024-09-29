#include "output.h"

using namespace std;

// C-tor: Initialize the BSON document
Output::Output(string pathToFileSave) : fileName(pathToFileSave)
{
    counter = 0;
    document = bson_new();

    // Add the sensors to the bson file
    bson_t sensors;
    BSON_APPEND_ARRAY_BEGIN(document, "Sensors", &sensors);

    Input &input = Input::getInstance();
    for (auto& [sensorId, sensorData] : input.sensors.items()) {
        bson_t bsonSensor;
        char key[16];
        snprintf(key, sizeof(key), "%d", stoi(sensorId));
        BSON_APPEND_DOCUMENT_BEGIN(&sensors, key, &bsonSensor);

        BSON_APPEND_INT32(&bsonSensor, "id", stoi(sensorId));
        BSON_APPEND_UTF8(&bsonSensor, "name", string(sensorData["name"]).c_str());
        BSON_APPEND_UTF8(&bsonSensor, "jsonPath", string(sensorData["pathToJson"]).c_str());

        bson_append_document_end(&sensors, &bsonSensor);
    }

    bson_append_array_end(document, &sensors);

    // Initialize the conditions
    BSON_APPEND_ARRAY_BEGIN(document, "Conditions", &conditions);
}

// Gets the singleton instance
Output &Output::getInstance()
{
    // Creates the instance if it does not exist
    if (!instance) {
        instance = unique_ptr<Output>(
            new Output("../../control/conditions.bson"));
    }
    // return Reference to the singleton `Output` instance
    return *instance;
}

// Static member to hold the single instance of `Output`
unique_ptr<Output> Output::instance = NULL;

logger Output::controlLogger("Control_gui");

// Functions to open bson for reading
// Use for checking right writing to BSON file
#pragma region helper

QJsonObject bsonToJsonObject(const bson_t *document)
{
    char *json = bson_as_json(document, nullptr);
    QJsonDocument jsonDoc =
        QJsonDocument::fromJson(QByteArray::fromRawData(json, strlen(json)));
    bson_free(json);
    return jsonDoc.object();
}

QJsonObject convertBsonToQJsonObject(const string &fileName)
{
    ifstream file(fileName, ios::binary | ios::ate);
    if (!file.is_open()) {
        Output::controlLogger.logMessage(logger::LogLevel::ERROR, "Failed to open file: " + fileName);
        return QJsonObject();  // Return an empty QJsonObject
    }

    streamsize size = file.tellg();
    file.seekg(0, ios::beg);

    vector<char> buffer(size);
    if (file.read(buffer.data(), size)) {
        const uint8_t *data = reinterpret_cast<const uint8_t *>(buffer.data());
        bson_t *doc = bson_new_from_data(data, size);
        if (doc) {
            QJsonObject jsonObject = bsonToJsonObject(doc);
            bson_destroy(doc);  // Clean up BSON document
            file.close();
            return jsonObject;
        }
        else {
            Output::controlLogger.logMessage(logger::LogLevel::ERROR, "Failed to create BSON document");
        }
    }
    else {
        Output::controlLogger.logMessage(logger::LogLevel::ERROR, "Failed to create file: " + fileName);
    }
    file.close();
    return QJsonObject();  // Return an empty QJsonObject
}

void printJson(QJsonObject jsonObject)
{
    QJsonDocument jsonDoc(jsonObject);
    QByteArray jsonBytes = jsonDoc.toJson();
    cout << jsonBytes.constData() << endl;
}

#pragma endregion

// Initialize a new BSON document for a condition and append it with a given condition string
void Output::addNewCondition(string strCondition)
{
    Output::controlLogger.logMessage(logger::LogLevel::INFO, "Add the `" + strCondition + "` condition to the BSON document.");
    currentCond =
        new bson_t();  // Initialize a new BSON document for the condition
    char key[16];
    snprintf(key, sizeof(key), "%d", counter++);  // Generate a unique key
    BSON_APPEND_DOCUMENT_BEGIN(&conditions, key,
                               currentCond);  // Begin condition document

    BSON_APPEND_UTF8(
        currentCond, "if",
        strCondition.c_str());  // Add condition string as "if" field
}

// Add an array of actions (ID and message) to the last condition in the BSON document
void Output::addActionsToLastCondition(vector<pair<int, string>> actions)
{
    Output::controlLogger.logMessage(logger::LogLevel::INFO, "Add actions to the last condition in BSON document.");
    bson_t send;
    BSON_APPEND_ARRAY_BEGIN(
        currentCond, "send",
        &send);  // Start the "send" array in the current condition
    int couterSend = 0;

    for (auto action : actions) {
        bson_t oneSend;
        char key[16];
        snprintf(key, sizeof(key), "%d",
                 couterSend++);  // Generate a unique key for each action
        BSON_APPEND_DOCUMENT_BEGIN(
            &send, key,
            &oneSend);  // Start an action document in the "send" array

        BSON_APPEND_INT32(&oneSend, "id", action.first);  // Add action ID
        BSON_APPEND_UTF8(&oneSend, "message",
                         action.second.c_str());  // Add action message

        bson_append_document_end(&send, &oneSend);  // End the action document
    }
    bson_append_array_end(currentCond, &send);  // End the "send" array

    bson_append_document_end(
        &conditions, currentCond);  // End the current condition document
}

// Saves the BSON document to a file and verifies the saved data by printing it as JSON
void Output::saveToFile()
{
    bson_append_array_end(document, &conditions);  // Finish the BSON document

    uint32_t length;
    uint8_t *buf = bson_destroy_with_steal(document, true,
                                           &length);  // Steal buffer for saving

    ofstream file(fileName, ios::binary | ios::trunc);  // Open file to save data
    if (file.is_open()) {
        file.write(reinterpret_cast<char *>(buf),
                   length);  // Write BSON data to file
        file.close();
        Output::controlLogger.logMessage(logger::LogLevel::INFO, "Successfully saved data to " + fileName);
    }
    else {
        Output::controlLogger.logMessage(logger::LogLevel::ERROR, "Failed to open file for writing: " + fileName);
    }

    bson_free(buf);  // Free the BSON buffer

    // Verify the saved file
    printJson(convertBsonToQJsonObject(
        fileName));  // Convert and print BSON as JSON
}