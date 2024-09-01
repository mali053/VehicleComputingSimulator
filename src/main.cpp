#include <QApplication>
#include "main_window.h"
#include "output.h"

int main(int argc, char *argv[])
{
    // QApplication app(argc, argv);

    // MainWindow window;
    // window.setWindowTitle("Build Condition");
    // window.resize(800, 600);
    // window.show();

    // return app.exec();

    Output output("my_bson.bson", {{1, "speed"}, {2, "tire pressure"}});
    output.addNewCondition("&([5]=(code,500),[4]<(status,700))");
    output.addActionsToLastCondition({{5, "decrease speed"}, {7, "slow down"}});
    output.saveToFile();
}