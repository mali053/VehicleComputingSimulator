#include <QApplication>
#include "main_window.h"

using namespace std;

int main(int argc, char *argv[])
{ 
    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle("Build Conditions");
    window.resize(1050, 930);
    window.show();

    Output::controlLogger.cleanUp();

    return app.exec();
}