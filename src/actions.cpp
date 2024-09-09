#include "actions.h"

Actions::~Actions() {}

void Actions::setupLogicalMembers()
{
    action = "send ";
    sensorList = {{"Speed", 1}, {"Tire Pressure", 2}, {"Communication", 3}, {"Camera", 4}};
    currentSensor = -1;
    currentMessage   = "";
}

void Actions::setupUi(QString showCondition) 
{
    QLabel* condLabel = new QLabel("");
    QFont fontComd("Arial", 15); 
    condLabel->setFont(fontComd);
    condLabel->setText(showCondition);
    
    actions = new QTextEdit("");
    actions->setReadOnly(true);
    actions->setFont(QFont("Arial", 10));

    label = new QTextEdit("");
    label->setReadOnly(true);
    QFont font("Arial", 25); 
    label->setFont(font);
    label->setText(action);

    QGroupBox *screenBox = new QGroupBox("");
    QVBoxLayout *screenLayout = new QVBoxLayout;

    screenLayout->addWidget(condLabel);
    screenLayout->addWidget(actions);
    screenLayout->addWidget(label);
    screenBox->setLayout(screenLayout);

    sensors = new QComboBox();
    sensors->addItem("Sensors"); 
    sensors->setCurrentIndex(0);
    sensors->setFixedSize(300, 50);
    sensors->setFont(QFont("Arial", 15));
    
 
    textBox = new QLineEdit();
    textBox->setFixedSize(300, 50);
    textBox->setStyleSheet("border: none;");
    textBox->setFont(QFont("Arial", 20));

    OKBtn = new QPushButton();
    QIcon iconOK("icons/check.png"); 
    OKBtn->setIcon(iconOK);
    OKBtn->setIconSize(QSize(48, 48)); 
    OKBtn->setFixedSize(50,50);
    OKBtn->setStyleSheet("border: none; ");

    addBtn = new QPushButton();
    QIcon iconAdd("icons/add_box.png"); 
    addBtn->setIcon(iconAdd);
    addBtn->setIconSize(QSize(78, 78)); 
    addBtn->setFixedSize(80,80);
    addBtn->setEnabled(false);
    addBtn->setStyleSheet("border: none;");


    QGroupBox *keyboardBox = new QGroupBox("");
    QVBoxLayout *keyboardLayout = new QVBoxLayout;
    
    QHBoxLayout *textLayout = new QHBoxLayout;
   
    textLayout->addWidget(sensors);

    QGroupBox *messageBox = new QGroupBox("");
    QHBoxLayout *messageLayout = new QHBoxLayout;

    messageLayout->addWidget(textBox);
    messageLayout->addWidget(OKBtn);

    messageBox->setLayout(messageLayout);

    textLayout->addWidget(messageBox);
    
    QSpacerItem* spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QHBoxLayout *addLayout = new QHBoxLayout;
    addLayout->addItem(spacer);
    addLayout->addWidget(addBtn);

    keyboardLayout->addLayout(textLayout);
    keyboardLayout->addLayout(addLayout);


    keyboardBox->setLayout(keyboardLayout);

    addCond = new QPushButton("add condition");
    addCond->setFixedSize(180,50);
    addCond->setFont(QFont("Arial", 15));
    addCond->setEnabled(false);

    finishBtn = new QPushButton("finish create");
    finishBtn->setFixedSize(180,50);
    finishBtn->setFont(QFont("Arial", 15));
    finishBtn->setEnabled(false);

    QHBoxLayout *nextLayout = new QHBoxLayout;

    nextLayout->addWidget(addCond);
    nextLayout->addWidget(finishBtn);
    nextLayout->addItem(spacer);

    this->addWidget(screenBox);   
    this->addWidget(keyboardBox);
    this->addLayout(nextLayout);


    for (const auto &sensor : sensorList) {
        sensors->addItem(sensor.first);
    }
}

void Actions::connectSignals() 
{
    connect(sensors, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &Actions::sensorSelectionHandler);
    connect(OKBtn, &QPushButton::clicked, this, &Actions::OKBtnHandler);
    connect(addBtn, &QPushButton::clicked, this, &Actions::addBtnHandler );
    connect(addCond, &QPushButton::clicked, this, &Actions::addCondHandler );

}

void Actions::sensorSelectionHandler(int index) 
{
    if(index > 0) {
        int selectedSensor = sensorList[sensors->currentText()];
        currentSensor = selectedSensor;
        if( !currentMessage.empty() ){
            action += QString::number(selectedSensor);
            addBtn->setEnabled(true);
            addCond->setEnabled(true);
            finishBtn->setEnabled(true);
        }
        else{
            action += "to sensor: " + QString::number(selectedSensor) + " message: ";
        }
        label->setText(action);
        sensors->setEnabled(false);
   }
}

void Actions::OKBtnHandler() 
{
    
    QString enteredText = textBox->text();
    textBox->setText("");
    currentMessage = enteredText.toStdString();
    if(currentSensor != -1){
        action += enteredText;
        addBtn->setEnabled(true);
        addCond->setEnabled(true);
        finishBtn->setEnabled(true);
    }
    else {
        action += "message: " + enteredText + " to sensor: ";
    }
    label->setText(action);
    OKBtn->setEnabled(false);
}

void Actions::addBtnHandler() 
{
    actions->append(action);
    messages.push_back({currentSensor,currentMessage});
    currentSensor = -1;
    currentMessage = "";
    action = "send ";
    label->setText(action);
    addBtn->setEnabled(false);
    addCond->setEnabled(false);
    finishBtn->setEnabled(false);
    sensors->setEnabled(true);
    OKBtn->setEnabled(true);
    sensors->setCurrentIndex(0);
}

void Actions::addCondHandler() 
{
//    mainWindow->goNext();
    delete this;
}
