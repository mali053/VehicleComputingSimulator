#include <QApplication>
#include "main_window.h"
// #include "output.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle("Build Conditions");
    window.resize(800, 600);
    window.show();

    return app.exec();

    // Output &output = Output::getInstance();
    // output.addNewCondition("&([5]=(code,500),[4]<(status,700))");
    // Output &output2 = Output::getInstance();
    // output2.addActionsToLastCondition({{5, "decrease speed"}, {7, "slow down"}});
    // output2.saveToFile();
}