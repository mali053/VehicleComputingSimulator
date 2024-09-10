#ifndef __ALERTER_H__
#define __ALERTER_H__

#include "detection_object_struct.h"

class Alerter {
private:
  char *makeAlertBuffer(const DetectionObject &detectionObject);
  void destroyAlertBuffer(char *buffer);

public:
  // TODO : pass all structs to external file
  void sendAlerts(const std::vector<DetectionObject> &output);
  void makeFileJSON();
};

#endif //__ALERTER_H__