#include "global_properties.h"

GlobalProperties::GlobalProperties()
{
    // Build the sensors according the json file
    Input::s_buildSensors(sensors);
}

GlobalProperties& GlobalProperties::getInstance()
{
    if (!instance) {
		instance = unique_ptr<GlobalProperties>(new GlobalProperties());
    }
    return *instance;
}

void GlobalProperties::resetInstance()
{
  instance.reset(new GlobalProperties());
}

unique_ptr<GlobalProperties> GlobalProperties::instance = NULL;