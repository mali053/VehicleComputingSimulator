#include "actions.h"

#define FONT_FAMILY "Arial"
using namespace std;

Actions::Actions(MainWindow *mainWindow, QString showCondition) : mainWindow(mainWindow)
{
    Output::controlLogger.logMessage(logger::LogLevel::INFO, "Initializing Actions window...");
    setupLogicalMembers();
    setupUi(showCondition);
    connectSignals();
}

//  Destructor for Actions class.
Actions::~Actions()
{
    Output::controlLogger.logMessage(logger::LogLevel::DEBUG, "Destroying Actions object");
    QLayoutItem *item;    
    // Loop through and remove all items from the layout
    while ((item = this->takeAt(0)) != nullptr) {
        QWidget *widget = item->widget(); // Retrieve the widget (if any)
        if (widget) {
            widget->setParent(nullptr); // Remove the widget from the layou
        }
        delete item; // Delete the layout item
    }
    delete finishBtn; // Delete the 'Finish' button
    delete addCond;  // Delete the 'Add Condition' button
}

// Sets up the initial logical members for the Actions class.
void Actions::setupLogicalMembers()
{
    action = "send "; // Default action string
    Input &input = Input::getInstance();
    for (auto &[sensorId, sensorData] : input.sensors.items()) 
        sensorList[QString::fromStdString(sensorData["name"])] = stoi(sensorId);
    currentSensor = -1; // No sensor selected
    currentMessage = ""; // No message yet
}

 // Sets up the UI components of the Actions window.
void Actions::setupUi(QString showCondition)
{
    // Create a label to show the current condition
    QLabel *condLabel = new QLabel("");
    QFont fontComd(FONT_FAMILY, 15);
    condLabel->setFont(fontComd);
    condLabel->setText(showCondition);

    // Create a read-only QTextEdit to display actions
    actions = new QTextEdit("");
    actions->setReadOnly(true);
    actions->setFont(QFont(FONT_FAMILY, 10));

    // Create a label for the current action string
    label = new QTextEdit("");
    label->setReadOnly(true);
    QFont font(FONT_FAMILY, 25);
    label->setFont(font);
    label->setText(action);

    // Create a screen layout to hold the condition and action labels
    QGroupBox *screenBox = new QGroupBox("");
    QVBoxLayout *screenLayout = new QVBoxLayout;

    screenLayout->addWidget(condLabel);
    screenLayout->addWidget(actions);
    screenLayout->addWidget(label);
    screenBox->setLayout(screenLayout);

    // Create a dropdown (QComboBox) for selecting sensors
    sensors = new QComboBox();
    sensors->addItem("Sensors");
    sensors->setItemData(0, QVariant(0),
                                   Qt::UserRole - 1);
    sensors->setCurrentIndex(0);
    sensors->setFixedSize(300, 50);
    sensors->setFont(fontComd);

    // Create a text box for user input (messages)
    textBox = new QLineEdit();
    textBox->setFixedSize(300, 50);
    textBox->setStyleSheet("border: none;");
    textBox->setFont(QFont(FONT_FAMILY, 20));

    // Create an OK button with an icon
    OKBtn = new QPushButton();
    QIcon iconOK("icons/check.png");
    OKBtn->setIcon(iconOK);
    OKBtn->setIconSize(QSize(48, 48));
    OKBtn->setFixedSize(50, 50);
    OKBtn->setStyleSheet("border: none; ");

    // Create an Add button with an icon, initially disabled
    addBtn = new QPushButton();
    QIcon iconAdd("icons/add_box.png");
    addBtn->setIcon(iconAdd);
    addBtn->setIconSize(QSize(78, 78));
    addBtn->setFixedSize(80, 80);
    addBtn->setEnabled(false);
    addBtn->setStyleSheet("border: none;");

    // Create layouts for the keyboard and text input area
    QGroupBox *keyboardBox = new QGroupBox("");
    QVBoxLayout *keyboardLayout = new QVBoxLayout;
    QHBoxLayout *textLayout = new QHBoxLayout;
    textLayout->addWidget(sensors);

    // Layout for the message input area and OK button
    QGroupBox *messageBox = new QGroupBox("");
    QHBoxLayout *messageLayout = new QHBoxLayout;
    messageLayout->addWidget(textBox);
    messageLayout->addWidget(OKBtn);
    messageBox->setLayout(messageLayout);
    textLayout->addWidget(messageBox);

    // Spacer and add button layout
    QSpacerItem *spacer =
        new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    QHBoxLayout *addLayout = new QHBoxLayout;
    addLayout->addItem(spacer);
    addLayout->addWidget(addBtn);

    // Add layouts to the keyboard layout
    keyboardLayout->addLayout(textLayout);
    keyboardLayout->addLayout(addLayout);
    keyboardBox->setLayout(keyboardLayout);

    // Create layout for the next buttons (Add Condition and Finish)
    QHBoxLayout *nextLayout = new QHBoxLayout;
    addCond = new QPushButton("add condition");
    addCond->setFixedSize(180, 50);
    addCond->setFont(fontComd);
    addCond->setEnabled(false);

    finishBtn = new QPushButton("finish create");
    finishBtn->setFixedSize(180, 50);
    finishBtn->setFont(fontComd);
    finishBtn->setEnabled(false);

    nextLayout->addItem(spacer);
    nextLayout->addWidget(addCond);
    nextLayout->addWidget(finishBtn);

    // Add all components to the main layout
    this->addWidget(screenBox);
    this->addWidget(keyboardBox);
    this->addLayout(nextLayout);

    // Populate the sensor dropdown with available sensors
    for (const auto &sensor : sensorList) {
        sensors->addItem(sensor.first);
        Output::controlLogger.logMessage(logger::LogLevel::DEBUG, "Added sensor to combo box: " + sensor.first.toStdString());
    }
}

// Connects signals and slots for the various UI components.
void Actions::connectSignals()
{
    connect(sensors, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &Actions::sensorSelectionHandler);
    connect(OKBtn, &QPushButton::clicked, this, &Actions::OKBtnHandler);
    connect(addBtn, &QPushButton::clicked, this, &Actions::addBtnHandler);
    connect(addCond, &QPushButton::clicked, this, &Actions::addCondHandler);
    connect(finishBtn, &QPushButton::clicked, this, &Actions::finishBtnHandler);
}

// Updates the current action with the selected sensor and enables further options.
void Actions::sensorSelectionHandler(int index)
{
    Output::controlLogger.logMessage(logger::LogLevel::INFO,  "Sensor selected:" + sensors->currentText().toStdString());
    if (index > 0) {
        int selectedSensor = sensorList[sensors->currentText()];
        currentSensor = selectedSensor;
        if (!currentMessage.empty()) {
            action += QString::number(selectedSensor);
            addBtn->setEnabled(true);
            addCond->setEnabled(true);
            finishBtn->setEnabled(true);
        }
        else {
            action +=
                "to sensor: " + QString::number(selectedSensor) + " message: ";
        }
        label->setText(action);
        sensors->setEnabled(false);
    }
}

//Updates the action with the message entered by the user.
void Actions::OKBtnHandler()
{
    Output::controlLogger.logMessage(logger::LogLevel::INFO, "Button OK clicked");
    QString enteredText = textBox->text();
    textBox->setText("");
    currentMessage = enteredText.toStdString();
    if (currentSensor != -1) {
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
    sensors->setCurrentIndex(0);
}

// Appends the current action to the actions list and resets the UI for the next action.
void Actions::addBtnHandler()
{
    Output::controlLogger.logMessage(logger::LogLevel::INFO, "Button add action clicked");
    actions->append(action);
    messages.push_back({currentSensor, currentMessage});
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

// Adds the current set of actions to the last condition.
void Actions::addCondHandler()
{
    Output::controlLogger.logMessage(logger::LogLevel::INFO, "Button add condition clicked");

    messages.push_back({currentSensor, currentMessage});

    // Add the actions to the condition in the bson
    Output &output = Output::getInstance();
    output.addActionsToLastCondition(messages);

    mainWindow->goNext();
}

// Finalizes the current condition and saves it to a BSON file.
void Actions::finishBtnHandler()
{
    Output::controlLogger.logMessage(logger::LogLevel::INFO, "Button finish create conditions");

    messages.push_back({currentSensor, currentMessage});

    // Add the actions to the condition in the bson
    Output &output = Output::getInstance();
    output.addActionsToLastCondition(messages);

    // Save the conditions to a bson file
    output.saveToFile();

    mainWindow->close();
}