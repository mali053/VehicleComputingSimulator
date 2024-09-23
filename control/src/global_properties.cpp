#include "global_properties.h"
using namespace std;

void handleMesseage(uint32_t senderId,void *data)
{
    GlobalProperties &instanceGP = GlobalProperties::getInstance();
    
    char * msg = "I got message";
    size_t dataSize = strlen(msg) + 1;
    cout << "MAIN : I got message from id " << senderId << endl;
    instanceGP.comm->sendMessage((void*)msg, dataSize, senderId, instanceGP.srcID, false);
    instanceGP.sensors[senderId]->handleMessage(data);
    cout << "\n****************\n" << endl;
    for (int cId : instanceGP.trueConditions)
        instanceGP.conditions[cId]->activateActions();

    free(data);
}

int readIdFromJson()
{
    // Read the json file
    ifstream f("config.json");

    // Check if the input is correct
    if (!f.is_open())
        cerr << "Failed to open config.json" << endl;

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
    return (*data)["ID"];
}

// Initializes the sensors based on a JSON file
GlobalProperties::GlobalProperties()
{
    // Build the sensors according the json file
    Input::s_buildSensors(sensors);

    srcID = readIdFromJson();
    // Creating the communication object with the callback function to process the data
    comm = new Communication(srcID, handleMesseage);
}

// Gets the singleton instance
GlobalProperties &GlobalProperties::getInstance()
{
    // Creates the instance if it does not exist
    if (!instance) {
        instance = unique_ptr<GlobalProperties>(new GlobalProperties());
    }
    // return Reference to the singleton `GlobalProperties` instance
    return *instance;
}

// Resets the singleton instance
void GlobalProperties::resetInstance()
{
    // Creates a new instance, replacing the existing one
    instance.reset(new GlobalProperties());
}

// Static member to hold the single instance of `GlobalProperties`
unique_ptr<GlobalProperties> GlobalProperties::instance = nullptr;