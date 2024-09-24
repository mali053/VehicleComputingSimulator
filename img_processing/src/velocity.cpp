
#include "../include/velocity.h"

using namespace std;
using namespace cv;

void Velocity::init(double frameTimeDiff)
{
    this->frameTimeDiff = frameTimeDiff;
}
void Velocity::returnVelocities(vector<ObjectInformation> &objects)
{
    for (auto &object : objects) {
        calculateVelocity(object);
    }
}
void Velocity::calculateVelocity(ObjectInformation &object)
{
    double distanceAvg = averageDistanceChange(object);
    if(distanceAvg!=-1){
        double velocity = distanceAvg / this->frameTimeDiff;
        object.velocity = velocity;
    }
}

double Velocity:: averageDistanceChange(ObjectInformation obj) const
{
    if (obj.prevDistances.size() < 2)
        return -1;
    double totalChange = 0.0;
    for (size_t i = 1; i < obj.prevDistances.size(); ++i) {
        totalChange += (obj.prevDistances[i] - obj.prevDistances[i - 1]);
    }
    return totalChange / (obj.prevDistances.size() - 1); 
}
