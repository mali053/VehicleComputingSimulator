#ifndef __CONDITION__
#define __CONDITION__

#include <QApplication>
#include <QPushButton>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QTimer>
#include <QStyleOptionViewItem>
#include <QPainter>
#include <QRandomGenerator>
#include <QLineEdit>
#include <QGroupBox>
#include <QTextEdit>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QDoubleSpinBox>
#include <QSpinBox>

#include <nlohmann/json.hpp>

#include <map>
#include <stack>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <math.h>
#include "main_window.h"
#include "input.h"
#include "output.h"

using json = nlohmann::json;
class MainWindow;

// Class that will be a layout in the main window
// Contain the condition configuration
class Condition : public QVBoxLayout {
    Q_OBJECT
private:
    // Logical members

    // Father window pointer
    MainWindow *mainWindow;
    // Variables contains the sensors and operators
    std::map<QString, int> sensorList;
    std::map<QString, QString> operatorsMap;
    std::vector<QString> operatorList;
    // Internal and showing conditions
    QString showCondition;
    QString condition;
    // Indexes for the conditions
    int ind;
    int indCondition;
    bool isCursorVisible;

    // Variables contains the sensor and the operator types
    std::pair<QString, int> typeCurrent;
    std::stack<std::pair<QString, int>> layersStack;
    
    // Labels for showing the conditions
    QTextEdit *label;

    // Cursor for label- for coloring it
    QTextCursor *cursor;

    // Formats for different colors
    QTextCharFormat formatRed;
    QTextCharFormat formatGreen;
    QTextCharFormat formatBlue;

    // Keyboard buttons
    QPushButton *andBtn;
    QPushButton *orBtn;
    QPushButton *skip;
    QPushButton *reset;
    // Objects for the sensors and basic conditions
    QLineEdit *textBox;
    QSpinBox *spinBox;
    QDoubleSpinBox *doubleSpinBox;
    QComboBox *boolBox;
    QPushButton *submit;
    QComboBox *operators;
    QComboBox *sensors;
    QComboBox *sensorsFields;
    // Button for move the other window
    QPushButton *selectActions;

public:
    static logger controlLogger;

    // C-tor that setup the layout
    explicit Condition(MainWindow *mainWindow);
   
    // D-tor that handles cleanup and saving of conditions
    ~Condition();

private:
    // Setup functions
    void setupLogicalMembers();
    void setupUi();
    void connectSignals();

    // Functions for the UI logic
    void updateDisplay();
    void buttonClickHandler(QPushButton *button);
    void skipHandler();
    void resetButtonState();
    void sensorSelectionHandler(int index);
    void updateSensorComboBoxState();
    void fieldSelectionHandler(int index);
    void operatorSelectionHandler(int index);
    void submitHandler();
    void updateSkipButtonState();
    void updateButtonVisible();
    void updateColors();
    void coverInputBoxes();
    void updateOperatorComboBox();


    friend class basicCondition_buildCondition_Test ;
    friend class oneOperatorDifferentSensors_buildCondition_Test ;
    friend class operatorInOperator_buildCondition_Test ;
    friend class oneOperatorOneSensor_buildCondition_Test ;
    friend class differentOperatorsDifferentSensors_buildCondition_Test ;
};

#endif // __CONDITION__