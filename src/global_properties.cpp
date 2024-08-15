#include "global_properties.h"

// Initializes the sensors based on a JSON file
GlobalProperties::GlobalProperties()
{
    // Build the sensors according the json file
    Input::s_buildSensors(sensors);
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
unique_ptr<GlobalProperties> GlobalProperties::instance = NULL;