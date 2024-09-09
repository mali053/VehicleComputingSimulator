#ifndef ACTIONS
#define ACTIONS

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
#include <QIcon>
#include <QTextEdit>
#include <QDir>


#include <map>
#include <vector>
#include <stack>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <math.h>

#include "main_window.h"
using namespace std;

class MainWindow;

class Actions :  public QVBoxLayout {
    Q_OBJECT
private:
    MainWindow *mainWindow;
    map<QString, int> sensorList;
    vector<pair<int,string>> messages;
    int currentSensor; 
    string currentMessage;

    QString action;
    QTextEdit *actions;
    QTextEdit *label;
    QComboBox *sensors;
    QLineEdit *textBox;
    QPushButton *OKBtn;
    QPushButton *addBtn;
    QPushButton *addCond;
    QPushButton *finishBtn;


    // Q type

public:
    Actions(MainWindow *mainWindow, QString showCondition)
    {
        this->mainWindow = mainWindow;
        setupLogicalMembers();
        setupUi(showCondition);
        connectSignals();
    }
    ~Actions();

private:
    void setupLogicalMembers();
    void setupUi(QString showCondition);
    void connectSignals();
    void sensorSelectionHandler(int index);
    void OKBtnHandler();
    void addBtnHandler();
};
#endif