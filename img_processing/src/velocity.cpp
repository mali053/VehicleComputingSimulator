
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
    //if(object.prevPosition.y < object.position.y){
    // Calculate the centers of the objects
    // cv::Point2f prevCenter(object.prevPosition.x,
    //   object.prevPosition.y);
    //cv::Point2f currentCenter(object.position.x ,
    //  object.position.y );
    // std::cout << "Prev Center: " << prevCenter << ", Current Center: " << currentCenter << std::endl;
    // Calculation of the distance between the centers
    //double distance = cv::norm(currentCenter - prevCenter);
    if (object.prevDistance == -1.0)
        return;
    double distance = object.distance - object.prevDistance;
    double velocity = distance / this->frameTimeDiff;
    object.prevVelocity = object.velocity;
    object.velocity = velocity;
}
