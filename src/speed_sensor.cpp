#include "speed_sensor.h"

// Implement the doAction function
void SpeedSensor::doAction(string action)
{
    //void Send_async(void* data, size_t size, uint srcId, uint dstId, *callback)
    //Send_async(action, sizeof(action), 0, this->id);
    cout << "I sensor " << id << " do " << action << endl;
}
