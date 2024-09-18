#ifndef __ACTIONS__
#define __ACTIONS__

#include <QApplication>
#include <QPushButton>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <map>
#include <vector>
#include <iostream>

#include "main_window.h"
#include "output.h"

class MainWindow;

// UI layout for creating actions linked to sensors.
class Actions : public QVBoxLayout {
    Q_OBJECT
private:
    MainWindow *mainWindow; // Pointer to the main window controlling UI flow.
    std::map<QString, int> sensorList; // Map of available sensors and their IDs.
    std::vector<std::pair<int, std::string>> messages; // Stores sensor ID and message.
    int currentSensor; // Currently selected sensor ID.
    QString action; // Stores the current action as a string.
    std::string currentMessage; // Stores the current message.

    // UI components for user interaction.
    QTextEdit *actions; 
    QTextEdit *label; 
    QComboBox *sensors; 
    QLineEdit *textBox;
    QPushButton *OKBtn, *addBtn, *addCond, *finishBtn; 

public:
    // Constructor: Initializes UI and connects signals.
    Actions(MainWindow *mainWindow, QString showCondition);
    
    // Destructor: Cleans up allocated resources.
    ~Actions();

private:
    // Initializes logical members and default values.
    void setupLogicalMembers();

    // Sets up the user interface components.
    void setupUi(QString showCondition);

    // Connects signals to their respective slots.
    void connectSignals();

    // Handles sensor selection from the combo box.
    void sensorSelectionHandler(int index);

    // Handles the OK button click event.
    void OKBtnHandler();

    // Handles the Add button click event.
    void addBtnHandler();

    // Handles the Add Condition button click event.
    void addCondHandler();

    // Handles the Finish button click event.
    void finishBtnHandler();

    friend class oneAction_buildActions_Test;
    friend class someActions_buildActions_Test;
};

#endif // __ACTIONS__