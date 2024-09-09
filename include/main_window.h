#ifndef MAIN_WINDOW
#define MAIN_WINDOW

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
#include "condition.h"
#include "actions.h"
using namespace std;

class MainWindow : public QWidget
{
    Q_OBJECT
private:
   QGroupBox* box;
   QVBoxLayout* boxLayout;
   QPushButton* next;
    // Q type



public:
    MainWindow(QWidget *parent = nullptr) : QWidget(parent) {
        setupUi();
        connectSignals(); 
    }
    ~MainWindow();
    void goNext();


private:
    void setupUi();
    void connectSignals();
};
#endif