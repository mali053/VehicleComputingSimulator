#ifndef __ALERTER_H__
#define __ALERTER_H__

#include "detection_object_struct.h"

class Alerter {
   public:
    std::vector<std::unique_ptr<char>> sendAlerts(
        const std::vector<DetectionObject> &output);
    void makeFileJSON();

   private:
    char *makeAlertBuffer(int type, double distance);
    bool isSendAlert(const DetectionObject &detectionObject);
    void destroyAlertBuffer(char *buffer);
};

#endif  //__ALERTER_H__