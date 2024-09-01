#include "main_window.h"

MainWindow::~MainWindow() {}

void MainWindow::setupUi()
{
    box = new QGroupBox();
    boxLayout = new Condition();
    box->setLayout(boxLayout);

    next = new QPushButton("next"); 

    QVBoxLayout *layout = new QVBoxLayout;

    layout->addWidget(box);
    layout->addWidget(next);
     
    this->setLayout(layout);
}

void MainWindow::connectSignals()
{
    connect(next, &QPushButton::clicked, this, &MainWindow::goNext);
}

void MainWindow::goNext() 
{
    // נקה את הפריסה הקודמת אם קיימת
    QLayout *oldLayout = box->layout();
    QString showCondition = static_cast<Condition*>(oldLayout)->getShowCondition();
    if (oldLayout) {
        // נקה את כל הווידג'טים בפריסה הישנה
        QLayoutItem *item;
        while ((item = oldLayout->takeAt(0))) {
            delete item->widget(); // מחק את הווידג'ט
            delete item; // מחק את פריט הפריסה
        }
        delete oldLayout; // מחק את הפריסה הישנה
    }

    // צור את הפריסה החדשה
    boxLayout = new Actions(showCondition);
    box->setLayout(boxLayout);
}
