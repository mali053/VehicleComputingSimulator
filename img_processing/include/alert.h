#ifndef __ALERT_H__
#define __ALERT_H__

#include <cstring>
#include <string>
#include <vector>

struct AlertDetails {
    // 1 bit to type of the message:
    // 0 - simple, distance  or
    // 1 - smart, collision prediction
    char messageType : 1;
    // 3 bits to alert level
    char level : 3;
    // 4 bits to object type
    char objectType : 4;
};

class Alert {
   public:
    Alert(bool messageType, int level, int type, float distance,
          float relativeVelocity = 0.0);
    Alert();
    int getMessageType() const;
    int getLevel() const;
    int getObjectType() const;
    float getObjectDistance() const;
    float getRelativeVelocity() const;
    std::vector<uint8_t> serialize();
    void deserialize(const char *buffer);

   private:
    AlertDetails alertDetails;
    float objectDistance;
    float relativeVelocity;
};

#endif  //__ALERT_H__