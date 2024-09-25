#include <QApplication>
#include "main_window.h"

using namespace std;

int main(int argc, char *argv[])
{
    Input &input = Input::getInstance();
    input.setPathToSensors("../test/sensors.json");
    
    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle("Build Conditions");
    window.resize(800, 600);
    window.show();

    Output::controlLogger.cleanUp();

    return app.exec();
}