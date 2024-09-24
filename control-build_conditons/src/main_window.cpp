#include "main_window.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    isConditionsWindow = true; // Start with the Condition layout.
    setupUi(); // Set up the user interface.
}

//Sets up the user interface for the MainWindow.
void MainWindow::setupUi()
{
    box = new QGroupBox(); // Create a new QGroupBox for the layout.
    boxLayout = new Condition(this); // Initialize with a Condition layout.

    box->setLayout(boxLayout); // Set the Condition layout inside the box.

    QVBoxLayout *layout = new QVBoxLayout; // Create the main layout for the window.

    layout->addWidget(box); // Add the QGroupBox to the main layout.

    this->setLayout(layout); //  Set the main layout for the window.

    Output::controlLogger.logMessage(logger::LogLevel::DEBUG, "Initialize MainWindow");
}

void MainWindow::saveCondition(QString condition)
{
    this->condition = condition; // Save the condition for future use.
}

// Handles switching between different layouts in the MainWindow.
void MainWindow::goNext()
{
    delete box->layout(); // Delete the current layout inside the QGroupBox.

    if (isConditionsWindow)
        boxLayout = new Actions(this, condition); // Switch to Actions layout, passing the saved condition.

    else
        boxLayout = new Condition(this); // Switch to Condition layout.

    box->setLayout(boxLayout);//  Set the new layout inside the QGroupBox.

    isConditionsWindow = !isConditionsWindow; // Toggle the state of isConditionsWindow.

    Output::controlLogger.logMessage(logger::LogLevel::DEBUG, "MainWindow::goNext");
}