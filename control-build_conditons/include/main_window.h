#ifndef __MAIN_WINDOW__
#define __MAIN_WINDOW__

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


// The MainWindow class represents the main window for the application.
class MainWindow : public QWidget {
    Q_OBJECT
private:
    QGroupBox *box; // Container for holding different layouts (e.g., actions or conditions).
    QVBoxLayout *boxLayout; // Current layout being displayed in the window.
    bool isConditionsWindow; // Tracks whether the current window is showing the Condition layout.
    QString condition; // Stores the current condition to be used in Actions layout.

public:
    // Constructor for MainWindow.
    MainWindow(QWidget *parent = nullptr);
    // Switches between the condition and action layout.
    void goNext(); 
    // Saves the current condition string.
    void saveCondition(QString condition); 

private:
    // Sets up the user interface for the main window.
    void setupUi(); 
};

#endif // __MAIN_WINDOW__