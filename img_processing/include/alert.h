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
    Alert(bool messageType, int level, int type, double distance,
          int carSpeed = 0, int objectSpeed = 0);
    Alert();
    int getMessageType() const;
    int getLevel() const;
    int getObjectType() const;
    double getObjectDistance() const;
    int getCarSpeed() const;
    int getObjectSpeed() const;
    std::vector<char> serialize();
    void deserialize(const char *buffer);

   private:
    AlertDetails alertDetails;
    double objectDistance;
    int carSpeed;
    int objectSpeed;
};

#endif  //__ALERT_H__