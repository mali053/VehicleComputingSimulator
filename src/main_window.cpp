#include "main_window.h"

MainWindow::~MainWindow() {}

void MainWindow::setupUi()
{
    box = new QGroupBox();
    boxLayout = new Condition(this);
    box->setLayout(boxLayout);

    QVBoxLayout *layout = new QVBoxLayout;

    layout->addWidget(box);
     
    this->setLayout(layout);
}

void MainWindow::connectSignals()
{
}

void MainWindow::saveCondition(QString condition) 
{
    this->condition = condition;
}

void MainWindow::goNext() 
{
    delete box->layout();
       
    if(isConditionsWindow)
        boxLayout = new Actions(this,condition);
    
    else 
        boxLayout = new Condition(this);
    
    box->setLayout(boxLayout);
    isConditionsWindow = !isConditionsWindow;
}
