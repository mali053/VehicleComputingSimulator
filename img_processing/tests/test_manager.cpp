#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>
#include "manager.h"
#include "jsonUtils.h"

using namespace std;
using namespace cv;

TEST(ManagerTest, tryManager)
{
    int processID = readFromJson("ID");
    Manager manager(processID);
    manager.init();
    manager.mainDemo();
}