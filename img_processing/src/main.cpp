#include <opencv2/opencv.hpp>
#include "manager.h"
#include "jsonUtils.h"

using namespace std;
using namespace cv;

int main()
{
    int processID = readFromJson("ID");
    Manager manager(processID);
    manager.init();
    manager.mainDemo();
    return 0;
}