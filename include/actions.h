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

#include <map>
#include <stack>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <math.h>
using namespace std;

class Actions :  public QVBoxLayout {
    Q_OBJECT
private:
    QString action;
    QLabel *label;
    QComboBox *sensors;
    QLineEdit *textBox;

    // Q type

public:
    Actions(QString showCondition)
    {
        setupLogicalMembers();
        setupUi(showCondition);
        connectSignals();
    }
    ~Actions();

private:
    void setupLogicalMembers();
    void setupUi(QString showCondition);
    void connectSignals();
};
#endif