#ifndef __ALERTER_H__
#define __ALERTER_H__

#include "object_information_struct.h"

class Alerter {
   public:
    std::vector<std::unique_ptr<char>> sendAlerts(
        const std::vector<ObjectInformation> &output);
    void makeFileJSON();
    static const int MIN_LEGAL_DISTANCE = 5000;

   private:
    char *makeAlertBuffer(int type, float distance, float relativeVelocity);
    bool isSendAlert(const ObjectInformation &objectInformation);
    void destroyAlertBuffer(char *buffer);
};

#endif  //__ALERTER_H__