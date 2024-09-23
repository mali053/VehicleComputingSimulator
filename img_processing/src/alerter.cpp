#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include "alerter.h"
#include "alert.h"
#include "manager.h"

using json = nlohmann::json;
using namespace std;

// create buffer from alert object
char *Alerter::makeAlertBuffer(int type, double distance)
{
    Alert alert(false, (int)distance, type, distance);
    vector<char> serialized = alert.serialize();
    char *buffer = new char[serialized.size()];
    copy(serialized.begin(), serialized.end(), buffer);
    return buffer;
}

void Alerter::destroyAlertBuffer(char *buffer)
{
    delete[] buffer;
}

// create alerts buffer to send
vector<unique_ptr<char>> Alerter::sendAlerts(
    const vector<ObjectInformation> &output)
{
    vector<unique_ptr<char>> alerts;
    for (const ObjectInformation &objectInformation : output) {
        if (isSendAlert(objectInformation)) {
            char *alertBuffer =
                makeAlertBuffer(static_cast<int>(objectInformation.type),
                                objectInformation.distance);
            alerts.push_back(unique_ptr<char>(alertBuffer));
        }
    }
    return alerts;
}

// Check whether to send alert
bool Alerter::isSendAlert(const ObjectInformation &objectInformation)
{
    return objectInformation.distance < MIN_LEGAL_DISTANCE;
}

// create json file form the alert buffer
void Alerter::makeFileJSON()
{
    json j;
    j["endianness"] = "little";
    j["fields"] = json::array();
    // field AlertDetails
    json alertDetailsJson;
    alertDetailsJson["name"] = "AlertDetails";
    alertDetailsJson["size"] = 8;
    alertDetailsJson["type"] = "bit_field";
    alertDetailsJson["fields"] = json::array(
        {{{"name", "MessageType"}, {"size", 1}, {"type", "unsigned_int"}},
         {{"name", "Level"}, {"size", 3}, {"type", "unsigned_int"}},
         {{"name", "ObjectType"}, {"size", 4}, {"type", "unsigned_int"}}});
    // add AlertDetails to JSON
    j["fields"].push_back(alertDetailsJson);
    // add ObjectDistance
    json objectDistanceJson;
    objectDistanceJson["name"] = "ObjectDistance";
    objectDistanceJson["size"] = 32;
    objectDistanceJson["type"] = "float_fixed";
    j["fields"].push_back(objectDistanceJson);
    // add CarSpeed
    json carSpeedJson;
    carSpeedJson["name"] = "CarSpeed";
    carSpeedJson["size"] = 32;
    carSpeedJson["type"] = "unsigned_int";
    j["fields"].push_back(carSpeedJson);
    // add ObjectSpeed
    json objectSpeedJson;
    objectSpeedJson["name"] = "ObjectSpeed";
    objectSpeedJson["size"] = 32;
    objectSpeedJson["type"] = "unsigned_int";
    j["fields"].push_back(objectSpeedJson);
    // Write the JSON to the file
    ofstream output_file("../alert.json");
    if (output_file.is_open()) {
        // 4 = Indent level for easier reading
        output_file << j.dump(4);
        output_file.close();
    }
    else {
        LogManager::logErrorMessage(ErrorType::FILE_ERROR, "open file for writing");
    }
}