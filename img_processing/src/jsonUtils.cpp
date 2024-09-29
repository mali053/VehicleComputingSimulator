#include "jsonUtils.h"

using namespace std;
using json = nlohmann::json;

int readFromJson(const char *target)
{
    // Read the json file
    ifstream f("config.json");
    // Check if the input is correct
    if (!f.is_open()) {
        LogManager::logErrorMessage(ErrorType::FILE_ERROR,
                                    "failed to open config.json");
    }
    json *data = NULL;
    // Try parse to json type
    try {
        data = new json(json::parse(f));
    }
    catch (exception e) {
        LogManager::logErrorMessage(ErrorType::FILE_ERROR, e.what());
    }
    catch (...) {
    }
    return (*data)[target];
}