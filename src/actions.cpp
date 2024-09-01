#include "actions.h"

Actions::~Actions() {}

void Actions::setupLogicalMembers()
{
    action = "send to sensor ";
}

void Actions::setupUi(QString showCondition) 
{
    label = new QLabel("");
    QLabel* condLabel = new QLabel("");
    QFont font("Arial", 25); 
    QFont fontComd("Arial", 15); 
    label->setFont(font);
    condLabel->setFont(fontComd);
    label->setText(action);
    condLabel->setText(showCondition);
    QGroupBox *screenBox = new QGroupBox("");
    QVBoxLayout *screenLayout = new QVBoxLayout;

    screenLayout->addWidget(condLabel);
    screenLayout->addWidget(label);
    screenBox->setLayout(screenLayout);

    sensors = new QComboBox();
    sensors->addItem("Sensors"); 
    sensors->setCurrentIndex(0);

     
    textBox = new QLineEdit();

    QGroupBox *keyboardBox = new QGroupBox("");
    QVBoxLayout *keyboardLayout = new QVBoxLayout;

    keyboardLayout->addWidget(sensors);
    keyboardLayout->addWidget(textBox);

    keyboardBox->setLayout(keyboardLayout);

    this->addWidget(screenBox);   
    this->addWidget(keyboardBox);
}

void Actions::connectSignals() 
{

}
