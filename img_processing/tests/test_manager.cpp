#include <gtest/gtest.h>
#include <opencv2/opencv.hpp>
#include "manager.h"

using namespace std;
using namespace cv;

TEST(ManagerTest, tryManager)
{
    Manager manager;
    manager.init();
    manager.mainDemo();
}