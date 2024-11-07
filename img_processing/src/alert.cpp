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

std::vector<uint8_t> Alert::serialize()
{
    std::vector<uint8_t> buffer;
    // Serialize messageType, level, and objectType from AlertDetails
    char detailsByte = 0;
    detailsByte |= (alertDetails.messageType & 0x01)
                   << 7;                              // messageType (1 bit)
    detailsByte |= (alertDetails.level & 0x07) << 4;  // level (3 bits)
    detailsByte |= alertDetails.objectType & 0x0F;    // objectType (4 bits)
    buffer.push_back(detailsByte);
    // Serialize objectDistance (32 bits, float, little-endian)
    float distance = objectDistance;
    uint8_t distanceBytes[4];
    std::memcpy(distanceBytes, &distance, sizeof(float));
    buffer.insert(buffer.end(), distanceBytes, distanceBytes + 4);
    // Serialize relativeVelocity (32 bits, float, little-endian)
    float velocity = relativeVelocity;
    uint8_t velocityBytes[4];
    std::memcpy(velocityBytes, &velocity, sizeof(float));
    buffer.insert(buffer.end(), velocityBytes, velocityBytes + 4);
    // Ensure the total size is as expected based on your format
    return buffer;
}

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