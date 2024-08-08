#include "alerter.h"
#include "alert.h"

using namespace std;

char *Alerter::makeAlertBuffer(const DetectionObject &detectionObject) {
  Alert alert(false, 1, detectionObject.type, 0);
  vector<char> serialized = alert.serialize();
  char *buffer = new char[serialized.size()];
  std::copy(serialized.begin(), serialized.end(), buffer);
  return buffer;
}

void Alerter::destroyAlertBuffer(char *buffer) { delete[] buffer; }

void Alerter::sendAlerts(const vector<DetectionObject> &output) {
  for (DetectionObject detectionObject : output) {
    // TODO : send to function that check if send alert...
    // if the function return true:
    char *alertBuffer = makeAlertBuffer(detectionObject);
    // TODO : use send comunication function.
    destroyAlertBuffer(alertBuffer);
  }
}