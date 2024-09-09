#include "output.h"


// Initializes the sensors based on a JSON file
Output::Output(string pathToFileSave, map<int, string> sensorsMap) : fileName(pathToFileSave)
{
    counter = 0;
    document = bson_new();

    // Add the sensors to the bson file
    bson_t sensors;
    BSON_APPEND_ARRAY_BEGIN(document, "Sensors", &sensors);

    for(auto sensor : sensorsMap) {
        bson_t bsonSensor;
        char key[16];
        snprintf(key, sizeof(key), "%d", sensor.first);
        BSON_APPEND_DOCUMENT_BEGIN(&sensors, key, &bsonSensor);

        BSON_APPEND_INT32(&bsonSensor, "id", sensor.first);
        BSON_APPEND_UTF8(&bsonSensor, "name", sensor.second.c_str());

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
        instance = unique_ptr<Output>(new Output("my_bson.bson", {{1, "speed"}, {2, "tire pressure"}}));
    }
    // return Reference to the singleton `GlobalProperties` instance
    return *instance;
}

// Static member to hold the single instance of `GlobalProperties`
unique_ptr<Output> Output::instance = NULL;


#pragma region helper

// Functions to open bson for reading
QJsonObject bsonToJsonObject(const bson_t *document) 
{
    char *json = bson_as_json(document, nullptr);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(QByteArray::fromRawData(json, strlen(json)));
    bson_free(json);
    return jsonDoc.object();
}

QJsonObject loadSimulationData(const std::string &fileName)
{
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return QJsonObject(); // Return an empty QJsonObject
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size)) {
        const uint8_t *data = reinterpret_cast<const uint8_t *>(buffer.data());
        bson_t *doc = bson_new_from_data(data, size);
        if (doc) {
            QJsonObject jsonObject = bsonToJsonObject(doc);
            bson_destroy(doc); // Clean up BSON document
            file.close();
            return jsonObject;
        } else {
            std::cerr << "Failed to parse BSON document" << std::endl;
        }
    } else {
        std::cerr << "Failed to read file: " << fileName << std::endl;
    }
    file.close();
    return QJsonObject(); // Return an empty QJsonObject
}

void printJson(QJsonObject jsonObject) 
{
    QJsonDocument jsonDoc(jsonObject);
    QByteArray jsonBytes = jsonDoc.toJson();
    std::cout << jsonBytes.toStdString() << std::endl;
}

#pragma endregion


void Output::addNewCondition(string strCondition)
{
    currentCond = new bson_t();
    char key[16];
    snprintf(key, sizeof(key), "%d", counter++);
    BSON_APPEND_DOCUMENT_BEGIN(&conditions, key, currentCond);

    BSON_APPEND_UTF8(currentCond, "if", strCondition.c_str());
}

void Output::addActionsToLastCondition(map<int, string> actions)
{
    bson_t send;
    BSON_APPEND_ARRAY_BEGIN(currentCond, "send", &send);
    int couterSend = 0;

    for(auto action : actions){
        bson_t oneSend;
        char key[16];
        snprintf(key, sizeof(key), "%d", couterSend++);
        BSON_APPEND_DOCUMENT_BEGIN(&send, key, &oneSend);

        BSON_APPEND_INT32(&oneSend, "id", action.first);
        BSON_APPEND_UTF8(&oneSend, "message", action.second.c_str());

        bson_append_document_end(&send, &oneSend);
    }
    bson_append_array_end(currentCond, &send);
    
    bson_append_document_end(&conditions, currentCond);
}

void Output::saveToFile()
{
    bson_append_array_end(document, &conditions);

    uint32_t length;
    uint8_t *buf = bson_destroy_with_steal(document, true, &length);
    cout << "Document size: " << length << " bytes" << endl;

    std::ofstream file(fileName, std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<char *>(buf), length);
        file.close();
        std::cout << "Successfully saved data to " << fileName << std::endl;
    }
    else {
        std::cerr << "Failed to open file for writing: " << fileName << std::endl;
    }

    bson_free(buf);

    // Check correct writing
    printJson(loadSimulationData("my_bson.bson"));
}