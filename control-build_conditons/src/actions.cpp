#include "actions.h"

#define FONT_FAMILY "Arial"
using namespace std;

Actions::Actions(MainWindow *mainWindow, QString showCondition) : mainWindow(mainWindow)
{
    Output::controlLogger.logMessage(logger::LogLevel::INFO, "Initializing Actions window...");
    fillMessageHistory();
    setupLogicalMembers();
    setupUi(showCondition);
    connectSignals();
}

//  Destructor for Actions class.
Actions::~Actions()
{
    addNewMessagesToHistory();
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
    QFont fontComd(FONT_FAMILY, 20);
    condLabel->setFont(fontComd);
    condLabel->setText(showCondition);

    // Create a read-only QTextEdit to display actions
    actions = new QTextEdit("");
    actions->setReadOnly(true);
    actions->setFont(QFont(FONT_FAMILY, 15));
    actions->setStyleSheet(R"(
    QTextEdit{
        border: 8px solid rgb(254, 254, 254); 
        border-radius: 8px; 
        padding: 10px; 
    }
    )");

    // Create a label for the current action string
    label = new QTextEdit("");
    label->setReadOnly(true);
    QFont font(FONT_FAMILY, 30);
    label->setFont(font);
    label->setText(action);
    label->setStyleSheet(R"(
    QTextEdit {
        border:8px solid rgb(254, 254, 254); 
        border-radius: 8px; 
        padding: 10px; 
    }
    )");

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
    sensors->setFixedSize(370, 76);
    sensors->setFont(fontComd);
    sensors->setStyleSheet(R"(
     QComboBox{
       border: 4px solid rgb(243, 93, 178);
       border-radius: 8px; 
    }
    QComboBox:hover {
        border: 4px solid #2F75A0;
    }
    QComboBox:focus {
        border: 4px solid #2F75A0; 
    }
    QComboBox::item:selected {
        color: #2F75A0;
    }
    )");

    // Create a text box for user input (messages)
    textBox = new QLineEdit();
    textBox->setFixedHeight(50);  
    textBox->setPlaceholderText("message");
    textBox->setFocusPolicy(Qt::StrongFocus);
    textBox->setStyleSheet(R"(
        QLineEdit {
            border: none;
            border-bottom: 2px solid transparent;
        }
        QLineEdit:focus {
            border-bottom: 2px solid #2F75A0; 
        }
    )");

    textBox->setFont(QFont(FONT_FAMILY, 20));

    completer = new QCompleter(messageHistory);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    textBox->setCompleter(completer);

    QAbstractItemView* popup = completer->popup();
    popup->setFont(QFont(FONT_FAMILY, 17));

    // Create an OK button with an icon
    OKBtn = new QPushButton();
    QIcon iconOK("icons/check.png");
    OKBtn->setIcon(iconOK);
    OKBtn->setIconSize(QSize(55, 55));
    OKBtn->setFixedSize(58, 58);
    OKBtn->setStyleSheet("border: none; ");

    // Create an Add button with an icon, initially disabled
    addBtn = new QPushButton();
    QIcon iconAdd("icons/add_box.png");
    addBtn->setIcon(iconAdd);
    addBtn->setIconSize(QSize(87, 87));
    addBtn->setFixedSize(90, 90);
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
    messageBox->setStyleSheet(R"(
    QGroupBox {
        border: 4px solid rgb(243, 93, 178); 
        border-radius: 8px; 
    }
    )");
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
    addCond->setFixedSize(220, 60);
    addCond->setFont(fontComd);
    addCond->setEnabled(false);

    finishBtn = new QPushButton("finish create");
    finishBtn->setFixedSize(220, 60);
    finishBtn->setFont(fontComd);
    finishBtn->setEnabled(false);

    addCond->setStyleSheet(R"(
        QPushButton {
            background-color: #17a2b8;
            color: white;
            border-radius: 10px;
            border: none;
            font-weight: bold;
        }
        QPushButton:hover {
            border: 2px solid #17a2b8;
            background-color: transparent;
            color: #17a2b8;
        }
        QPushButton:disabled {
            background-color: rgba(23, 162, 184, 0.5);
            color: rgba(255, 255, 255, 0.6);
        }
    )");

    finishBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #17a2b8;
            color: white;
            border-radius: 10px;
            border: none;
            font-weight: bold;
        }
        QPushButton:hover {
            border: 2px solid #17a2b8;
            background-color: transparent;
            color: #17a2b8;
        }
        QPushButton:disabled {
            background-color: rgba(23, 162, 184, 0.5);
            color: rgba(255, 255, 255, 0.6);
        }
    )");

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
    if(!enteredText.isEmpty()){
            
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

        if (!messageHistory.contains(enteredText)) {
            newMessages.append(enteredText); 
            messageHistory.append(enteredText);  
            completer->setModel(new QStringListModel(messageHistory, completer));
        }

    }

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

//  Saves new messages to the history file.
void Actions::addNewMessagesToHistory() 
{
    QFile file("../resources/history.txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
    QTextStream out(&file);
    for (const QString& msg : newMessages) 
        out << msg << "\n";

    file.close();
}
}

//  Loads messages from the file into a list in memory.
void Actions::fillMessageHistory() 
{
    QFile file("../resources/history.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        messageHistory.append(line);
    }
    file.close();
}
}
