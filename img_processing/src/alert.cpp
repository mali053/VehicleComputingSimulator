#include "alert.h"
#include "manager.h"

using namespace std;

Alert::Alert(){};

Alert::Alert(bool messageType, int level, int type, float distance,
             float relativeVelocity)
    : alertDetails{messageType, static_cast<char>(level),
                   static_cast<char>(type)},
      objectDistance(distance),
      relativeVelocity(relativeVelocity){};

int Alert::getMessageType() const
{
    return alertDetails.messageType;
};

int Alert::getObjectType() const
{
    return alertDetails.objectType;
};

int Alert::getLevel() const
{
    return alertDetails.level;
};

float Alert::getObjectDistance() const
{
    return objectDistance;
};

float Alert::getRelativeVelocity() const
{
    return relativeVelocity;
}

vector<char> Alert::serialize()
{
    vector<char> buffer;
    // determine the size of the buffer
    buffer.reserve(sizeof(AlertDetails) + sizeof(float) * 2);

    // serialize alertDetails
    char *alertDetailsPtr = reinterpret_cast<char *>(&alertDetails);
    buffer.insert(buffer.end(), alertDetailsPtr,
                  alertDetailsPtr + sizeof(AlertDetails));

    // serialize distance
    char *distancePtr = reinterpret_cast<char *>(&objectDistance);
    buffer.insert(buffer.end(), distancePtr, distancePtr + sizeof(float));

    // serialize relativeVelocity
    char *relativeVelocityPtr = reinterpret_cast<char *>(&relativeVelocity);
    buffer.insert(buffer.end(), relativeVelocityPtr,
                  relativeVelocityPtr + sizeof(float));

    return buffer;
};

void Alert::deserialize(const char *buffer)
{
    int place = 0;
    // Deserialize alertDetails
    memcpy(&alertDetails, buffer, sizeof(AlertDetails));
    place += sizeof(AlertDetails);

    // Deserialize distance
    memcpy(&objectDistance, buffer + place, sizeof(float));
    place += sizeof(float);

    // Deserialize relativeVelocity
    memcpy(&relativeVelocity, buffer + place, sizeof(float));
};