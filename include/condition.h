#ifndef CONDITION
#define CONDITION

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


#include <map>
#include <stack>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <math.h>
#include "main_window.h"

#include "json.hpp"

using json = nlohmann::json;
using namespace std;
class MainWindow;

class Condition : public QVBoxLayout 
{
    Q_OBJECT
private:
    // Regular
    MainWindow *mainWindow;
    map<int, vector<QString>> sensorsFieldsList;
    map<QString, int> sensorList;
    map<QString, QString> operatorsMap;
    vector<QString> operatorList;
    QString showCondition;
    QString condition;
    int ind;           // המיקום ההתחלתי של האינדקס
    int indCondition;  // המיקום ההתחלתי של האינדקס
    
    bool isCursorVisible;    // מוצג או לא "I" משתנה שיאפשר לנו לדעת אם הסימון

    pair<QString, int> typeCurrent;
    stack<pair<QString, int>> layersStack;


    // Q type

    // יצירת QLabel להצגת הטקסט
    QTextEdit *label;
    QLabel *label1;

    QTextCursor *cursor;

    // הגדרת פורמטים לצבעים שונים
    QTextCharFormat formatRed;
    QTextCharFormat formatGreen;
    QTextCharFormat formatBlue;

    // יצירת כפתורים עם טקסט
    QPushButton *andBtn;
    QPushButton *orBtn;
    QPushButton *skip;
    QPushButton *reset;
    QLineEdit *textBox;
    QPushButton *submit;
    QComboBox *operators;
    QComboBox *sensors;
    QComboBox *sensorsFields;
    QPushButton *selectActions;

public:
   explicit Condition( MainWindow *mainWindow) {
        this->mainWindow = mainWindow;
        setupLogicalMembers();
        setupUi();
        connectSignals(); 
    }
    ~Condition();

private:
    void setupLogicalMembers();
    void setupUi();
    void connectSignals();
    void updateDisplay();

    void buttonClickHandler(QPushButton *button);
    void skipHandler();
    void resetButtonState();
    void sensorSelectionHandler(int index);
    void updateSensorComboBoxState();
    void operatorSelectionHandler(int index);
    void submitHandler();
    void updateSkipButtonState();
    void updateButtonVisible();
    void updateColors();

    void fillSensorsFields(map<int, string> pathesToJsonFiles);
    // This function should be written in another ststic class (like "input" in the main)
    vector<QString> getFieldsOfSensor(string psthToSensorJson);
};
#endif