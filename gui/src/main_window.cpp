#include <QMessageBox>
#include <QHBoxLayout>
#include <QFileInfo>
#include <QPixmap>
#include <QFileDialog>
#include <QTimer>
#include <QJsonDocument>
#include "process.h"
#include "main_window.h"
#include "draggable_square.h"
#include "process_dialog.h"
#include "frames.h"
#include "log_handler.h"
#include "compiler.h"

int sizeSquare = 120;
int rotationTimerIntervals = 100;
logger MainWindow::guiLogger("gui");

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), timer(nullptr)
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    QWidget *toolbox = new QWidget(this);
    toolboxLayout = new QVBoxLayout(toolbox);
    compileButton = new QPushButton("Compile", this);
    runButton = new QPushButton("Run", this);
    endButton = new QPushButton("End", this);
    timerButton = new QPushButton("Set Timer", this);
    timeInput = new QLineEdit(this);
    timeLabel = new QLabel("Enter time in seconds:", this);
    logOutput = new QTextEdit(this);
    QPushButton *chooseButton = new QPushButton("Choose Image", this);
    QPushButton *showSimulationButton =
        new QPushButton("Show\nSimulation", this);
    showSimulationButton->setStyleSheet(
        "QPushButton {"
        "border: 2px solid #8f8f91;"
        "border-radius: 25px;"
        "background-color: #d1d1d1;"
        "min-width: 50px;"
        "min-height: 50px;"
        "font-size: 14px;"
        "}"
        "QPushButton:pressed {"
        "background-color: #a8a8a8;"
        "}");
    QPushButton *loadSimulationButton =
        new QPushButton("LOAD\nSIMULATION", toolbox);
    loadSimulationButton->setFixedHeight(
        loadSimulationButton->sizeHint().height() * 1.5);

    timeLabel->hide();
    timeInput->hide();
    logOutput->setReadOnly(true);

    mainLayout->addWidget(toolbox);

    QPushButton *addProcessButton = new QPushButton("Add Process", toolbox);
    toolboxLayout->addWidget(addProcessButton);
    toolboxLayout->insertWidget(1, showSimulationButton);
    toolboxLayout->insertWidget(2, loadSimulationButton);
    toolboxLayout->addStretch();

    connect(addProcessButton, &QPushButton::clicked, this,
            &MainWindow::createNewProcess);
    connect(compileButton, &QPushButton::clicked, this,
            &MainWindow::compileProjects);
    connect(runButton, &QPushButton::clicked, this, &MainWindow::runProjects);
    connect(endButton, &QPushButton::clicked, this, &MainWindow::endProcesses);
    connect(timerButton, &QPushButton::clicked, this,
            &MainWindow::showTimerInput);
    connect(chooseButton, &QPushButton::clicked, this,
            &MainWindow::openImageDialog);
    connect(showSimulationButton, &QPushButton::clicked, this,
            &MainWindow::showSimulation);
    connect(loadSimulationButton, &QPushButton::clicked, this,
            &MainWindow::loadSimulation);
    // Create and set up the loading spinner (static PNG)
    loadingLabel = new QLabel(this);
    // Set up the loading spinner (rotating static image)
    loadingLabel->setFixedSize(80, 80);  // Set smaller fixed size (adjust as needed)
    loadingLabel->setScaledContents(true);  // Make sure the image scales with the label
    loadingPixmap = QPixmap("../loading.png");  // Path to the PNG image
    loadingLabel->setPixmap(loadingPixmap);
    loadingLabel->setAlignment(Qt::AlignCenter);
    loadingLabel->hide();  // Initially hidden

    // Initialize rotation-related variables
    rotationTimer = new QTimer(this);
    rotationAngle = 0;

    // Connect the timer to the rotation function
    connect(rotationTimer, &QTimer::timeout, this, &MainWindow::rotateImage);

    toolbox->setMaximumWidth(100);
    toolbox->setMinimumWidth(100);
    toolboxLayout->addWidget(loadingLabel); // Add the loading label to the toolbox layout (under the buttons)
    toolboxLayout->addWidget(compileButton);
    toolboxLayout->addWidget(runButton);
    runButton->setEnabled(false);
    toolboxLayout->addWidget(endButton);
    toolboxLayout->addWidget(timerButton);
    toolboxLayout->addWidget(timeLabel);
    toolboxLayout->addWidget(timeInput);
    toolboxLayout->addWidget(logOutput);
    toolboxLayout->addWidget(chooseButton);

    workspace = new QWidget(this);
    workspace->setStyleSheet("background-color: white;");

    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    mainLayout->addWidget(workspace);
    centralWidget->setLayout(mainLayout);

    stateManager = new SimulationStateManager(this);

    int id = 0;
    const QString styleSheet =
        "QWidget {"
        "  background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, "
        "    stop: 0 #FDE1E1, stop: 1 #D4A5FF);"  // Gradient from purple to pink
        "  border: 3px solid silver;"             // Silver-colored borders
        "  border-radius: 10px;"                  // Rounded corners
        "}";

    Process *mainProcess =
        new Process(id, "Bus_Manager", "../../main_bus/CMakeLists.txt", "QEMUPlatform");
    addProcessSquare(
        mainProcess,
        QPoint((id % 2) * (sizeSquare + 10), (id / 2) * (sizeSquare + 10)),
        sizeSquare, sizeSquare, styleSheet);
    addId(id++);
    Process *hsmProcess =
        new Process(id, "HSM", "path/to/hsm/directory/CMakeLists.txt", "QEMUPlatform");
    addProcessSquare(
        hsmProcess,
        QPoint((id % 2) * (sizeSquare + 10), (id / 2) * (sizeSquare + 10)),
        sizeSquare, sizeSquare, styleSheet);
    addId(id++);
    Process *logsDbProcess =
        new Process(id, "LogsDb", "path/to/LogsDb/directory/CMakeLists.txt", "QEMUPlatform");
    addProcessSquare(
        logsDbProcess,
        QPoint((id % 2) * (sizeSquare + 10), (id / 2) * (sizeSquare + 10)),
        sizeSquare, sizeSquare, styleSheet);
    addId(id++);
    Process *busManagerProcess =
        new Process(id, "Main", "path/to/Main/directory/CMakeLists.txt", "QEMUPlatform");
    addProcessSquare(
        busManagerProcess,
        QPoint((id % 2) * (sizeSquare + 10), (id / 2) * (sizeSquare + 10)),
        sizeSquare, sizeSquare, styleSheet);
    addId(id++);
}

MainWindow::~MainWindow()
{
    qDeleteAll(squares);
    if (timer) {
        delete timer;
    }
}

void MainWindow::createNewProcess()
{
    ProcessDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted && dialog.isValid()) {
        int id = dialog.getId();
        if (id <= 4) {
            QMessageBox::warning(this, "Invalid ID",
                                 "The ID must be greater than 4.");
            MainWindow::guiLogger.logMessage(
                logger::LogLevel::ERROR, "MainWindow", "createNewProcess",
                "Invalid ID entered: " + std::to_string(id));
            return;
        }
        if (!isUniqueId(id)) {
            QMessageBox::warning(this, "Non-unique ID",
                                 "The ID entered is already in use. Please "
                                 "choose a different ID.");
            MainWindow::guiLogger.logMessage(
                logger::LogLevel::ERROR, "MainWindow", "createNewProcess",
                "Non-unique ID entered: " + std::to_string(id));
            return;
        }
        Process *newProcess =
            new Process(id, dialog.getName(), dialog.getExecutionFile(),
                        dialog.getQEMUPlatform());
        addProcessSquare(newProcess);
        addId(id);
        MainWindow::guiLogger.logMessage(
            logger::LogLevel::INFO, "MainWindow", "createNewProcess",
            "New process created with ID: " + std::to_string(id));
    }
}

Process *MainWindow::getProcessById(int id)
{
    for (DraggableSquare *square : squares) {
        if (square->getId() == id) {
            return square->getProcess();
        }
    }
    return nullptr;  // Return nullptr if the process with the given ID is not found
}

void MainWindow::addProcessSquare(Process *&process)
{
    DraggableSquare *square = new DraggableSquare(workspace);
    square->setProcess(process);
    square->setId(process->getId());
    QPoint pos = squarePositions.value(process->getId(), QPoint(0, 0));
    square->move(pos);
    square->show();
    square->setDragStartPosition(pos);
    squarePositions[process->getId()] = pos;
    squares.push_back(square);

    createProcessConfigFile(process->getId(), process->getExecutionFile());
}

void MainWindow::addProcessSquare(Process *process, QPoint position, int width,
                                  int height, const QString &color)
{
    DraggableSquare *square =
        new DraggableSquare(workspace, color, width, height);

    square->setProcess(process);
    square->setId(process->getId());
    QPoint pos = squarePositions.value(process->getId(), position);
    square->move(pos);
    square->show();

    squarePositions[process->getId()] = pos;
    squares.push_back(square);
}

bool MainWindow::isUniqueId(int id)
{
    return !usedIds.contains(id);
}

void MainWindow::addId(int id)
{
    usedIds.insert(id);
}

void MainWindow::updateTimer()
{
    QString inputText = timeInput->text();
    bool ok = true;
    int time = 0;

    if (!inputText.isEmpty()) {
        time = inputText.toInt(&ok);
    }

    if (!ok || (time <= 0 && !inputText.isEmpty())) {
        QMessageBox::warning(this, "Invalid Input",
                             "Please enter a valid number of seconds.");
        timeInput->clear();
        MainWindow::guiLogger.logMessage(
            logger::LogLevel::ERROR, "MainWindow", "startProcesses",
            "Invalid timer input: " + inputText.toStdString());
        return;
    }

    // Set the text to be centered and styled for better clarity
    timeInput->setAlignment(Qt::AlignCenter);
    QFont font = timeInput->font();
    font.setPointSize(10);  // Increase font size for better visibility
    timeInput->setFont(font);

    if (time > 0) {
        logOutput->append("Timer started for " + QString::number(time) +
                          " seconds.");
        MainWindow::guiLogger.logMessage(
            logger::LogLevel::INFO, "MainWindow", "startProcesses",
            "Timer started for " + std::to_string(time) + " seconds");

        if (timer) {
            timer->stop();
            delete timer;
        }

        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this, time]() mutable {

            if (time > 0) {
                time--;
                timeInput->setText(QString::number(time));
                timeInput->setAlignment(Qt::AlignCenter);  // Keep text centered
            } else {
                timerTimeout();
            }
        });

        timer->start(1000);
        timeLabel->hide();
        timeInput->setEnabled(false);
    }
}

void MainWindow::endProcesses()
{
    enableAllButtons();
    hideLoadingIndicator();
    logOutput->append("Ending processes...");
    MainWindow::guiLogger.logMessage(
        logger::LogLevel::INFO,
        "MainWindow::endProcesses()   Ending processes");

    if (timer) {
        timer->stop();
        delete timer;
        timer = nullptr;
    }

    timeLabel->show();
    timeInput->clear();

    stateManager->saveSimulationState("simulation_state.bson", squares,
                                      currentImagePath);
    MainWindow::guiLogger.logMessage(logger::LogLevel::INFO,
                                     "MainWindow::endProcesses  Simulation "
                                     "data saved to simulation_state.bson");

    for (const QPair<QProcess *, int> &pair : runningProcesses) {
        QProcess *process = pair.first;
        int id = pair.second;
        if (process->state() != QProcess::NotRunning) {
            process->terminate();
            process->waitForFinished();
        }
        // Check if the process is already marked for deletion by deleteLater
        if (process && !process->parent()) {
            // Only delete if it hasn't already been marked for deletion
            delete process;
        }
    }
    runningProcesses.clear();
}

void MainWindow::stopProcess(int deleteId)
{
    for (int i = 0; i < runningProcesses.size(); ++i) {
        QProcess *process = runningProcesses[i].first;
        int id = runningProcesses[i].second;

        if (id == deleteId && id > 3) {
            if (process->state() != QProcess::NotRunning) {
                logOutput->append("Ending process...");
                process->terminate();
                process->waitForFinished();
            }

            process->deleteLater();
            break;
        }
    }
}

void MainWindow::showTimerInput()
{
    timeLabel->show();
    timeInput->show();

    // Center the text and set the style
    timeInput->setAlignment(Qt::AlignCenter);
    QFont font = timeInput->font();
    font.setPointSize(10);  // Increase font size for better visibility
    timeInput->setFont(font);

    // Connect to textChanged signal to ensure text stays centered
    connect(timeInput, &QLineEdit::textChanged, this, [this]() {
        timeInput->setAlignment(Qt::AlignCenter);
    });

    // Set input validator to ensure only numbers are entered
    QIntValidator* validator = new QIntValidator(0, 999999, this);
    timeInput->setValidator(validator);

    guiLogger.logMessage(
        logger::LogLevel::DEBUG,
        "showTimerInput() called: Timer input elements are shown and centered.");
}

void MainWindow::timerTimeout()
{
    logOutput->append("Timer timeout reached.");
    guiLogger.logMessage(logger::LogLevel::INFO, "Timer timeout reached.");

    endProcesses();
}

void MainWindow::openImageDialog()
{
    QString imagePath = QFileDialog::getOpenFileName(
        this, tr("Select Image"), "", tr("Image Files (*.png *.jpg *.jpeg)"));

    if (!imagePath.isEmpty()) {
        currentImagePath = imagePath;
        QPixmap pixmap(imagePath);

        if (!pixmap.isNull()) {
            // Clear the workspace before adding the new image
            QLayout *layout = workspace->layout();
            if (layout) {
                QLayoutItem *item;
                while ((item = layout->takeAt(0)) != nullptr) {
                    delete item->widget();
                    delete item;
                }
            }

            // Create a new QLabel to display the image as background
            QLabel *backgroundLabel = new QLabel(workspace);
            backgroundLabel->setPixmap(pixmap.scaled(workspace->size(),
                                                     Qt::IgnoreAspectRatio,
                                                     Qt::SmoothTransformation));
            backgroundLabel->setGeometry(workspace->rect());
            backgroundLabel->setScaledContents(true);
            backgroundLabel->setAttribute(Qt::WA_TranslucentBackground);
            backgroundLabel->lower();
            backgroundLabel->show();

            // Add a new layout to the workspace
            QVBoxLayout *newLayout = new QVBoxLayout(workspace);
            workspace->setLayout(newLayout);

            guiLogger.logMessage(
                logger::LogLevel::INFO,
                "openImageDialog() called: Image loaded and displayed.");
        }
        else {
            guiLogger.logMessage(
                logger::LogLevel::ERROR,
                "openImageDialog() failed: Unable to load image from path " +
                    imagePath.toStdString());
        }
    }
    else {
        guiLogger.logMessage(
            logger::LogLevel::INFO,
            "openImageDialog() canceled: No image path selected.");
    }
}

void MainWindow::showSimulation()
{
    QString filePath = "log_file.log";
    logHandler.readLogFile(filePath);
    logHandler.analyzeLogEntries(this, "simulation_state.bson");

    Frames* frames = new Frames(logHandler);  // Initialize Frames
    QVBoxLayout *framesLayout = new QVBoxLayout(workspace);
    framesLayout->addWidget(frames);
    workspace->setLayout(framesLayout);
}

void MainWindow::loadSimulation()
{
    SimulationStateManager *simManager;
    simManager = new SimulationStateManager();
    simManager->loadSimulationState("simulation_state.bson");
    for (auto sq : squares) {
        delete (sq);
    }
    squares.clear();
    squarePositions.clear();
    if (!simManager->data.squares.empty()) {
        for (auto sqr : simManager->data.squares) {
            Process *process = new Process(
                sqr->getProcess()->getId(), sqr->getProcess()->getName(),
                sqr->getProcess()->getExecutionFile(),
                sqr->getProcess()->getQEMUPlatform());
            addProcessSquare(process, sqr->pos(), sqr->width(), sqr->height(),
                             sqr->styleSheet());
            addId(sqr->getId());
        }
    }
}

QString MainWindow::getExecutableName(const QString &buildDirPath)
{
    QDir buildDir(buildDirPath);

    // Check if the directory exists
    if (!buildDir.exists()) {
        QMessageBox::critical(this, "Directory Error",
                              "The specified build directory does not exist.");
        guiLogger.logMessage(
            logger::LogLevel::ERROR,
            "getExecutableName() failed: Directory does not exist: " +
                buildDirPath.toStdString());
        return QString();
    }

    QStringList files = buildDir.entryList(QDir::Files | QDir::NoSymLinks);

    // If the directory is empty or has no files
    if (files.isEmpty()) {
        QMessageBox::critical(
            this, "File Error",
            "No files found in the specified build directory.");
        guiLogger.logMessage(
            logger::LogLevel::ERROR,
            "getExecutableName() failed: No files found in directory: " +
                buildDirPath.toStdString());
        return QString();
    }

    foreach (const QString &file, files) {
        QFileInfo fileInfo(buildDir.filePath(file));

        // Skip files with a suffix (i.e., non-executables)
        if (!fileInfo.suffix().isEmpty())
            continue;

        // If the file is executable, return its name
        if (fileInfo.isExecutable()) {
            return fileInfo.fileName();
        }
    }

    // No executable found
    QMessageBox::critical(
        this, "Executable Not Found",
        "No executable file found in the specified build directory.");
    guiLogger.logMessage(
        logger::LogLevel::ERROR,
        "getExecutableName() failed: No executable file found in directory: " +
            buildDirPath.toStdString());
    return QString();
}

void MainWindow::compileProjects()
{
    guiLogger.logMessage(logger::LogLevel::INFO,
                         "Compiling and running projects started.");

    // Disable the run button until compilation finishes
    runButton->setEnabled(false);

    // Clear previous running processes
    for (const QPair<QProcess *, int> &pair : runningProcesses) {
        QProcess *process = pair.first;
        process->terminate();
        process->waitForFinished();
        delete process;
    }

    runningProcesses.clear();
    bool compileSuccessful = true;  // Track if all compilations succeed

    QList<QThread *> threads;

    QSet<QString> uniquePaths;  // Set to hold unique paths
    for (DraggableSquare *square : squares) {
        QString executionFilePath = square->getProcess()->getExecutionFile();
        uniquePaths.insert(executionFilePath);
    }

    for (QString executionFilePath : uniquePaths) {
        Compiler *compiler =
            new Compiler(executionFilePath, &compileSuccessful, this);
        connect(compiler, &Compiler::logMessage, this,
                [this](const QString &message) {
                    guiLogger.logMessage(logger::LogLevel::ERROR,
                                         message.toStdString());
                });
        compiler->start();
        threads.append(compiler);
    }

    // Wait for all threads to finish
    for (QThread *thread : threads) {
        thread->wait();
        delete thread;  // Clean up threads after completion
    }

    if (compileSuccessful) {
        runButton->setEnabled(true);
        logOutput->append(
            "Compilation completed successfully. You can now run the "
            "projects.");
    }
    else {
        logOutput->append("Compilation failed. Please check the logs.");
    }
}

void MainWindow::runProjects()
{
    disableButtonsExceptEnd();
    showLoadingIndicator();
    updateTimer();
    for (DraggableSquare *square : squares) {
        QString executionFilePath = square->getProcess()->getExecutionFile();

        if (executionFilePath.endsWith(".sh")) {
            QProcess *scriptProcess = new QProcess(this);
            scriptProcess->start("bash", QStringList() << executionFilePath);
            connect(
                scriptProcess, &QProcess::readyReadStandardOutput,
                [this, scriptProcess]() {
                    logOutput->append(scriptProcess->readAllStandardOutput());
                });
            connect(
                scriptProcess, &QProcess::readyReadStandardError,
                [this, scriptProcess]() {
                    logOutput->append(scriptProcess->readAllStandardError());
                });
        }
        else {
            QDir cmakeDir(
                QFileInfo(executionFilePath)
                    .absolutePath());  // Get the directory containing CMakeLists.txt
            QString buildDirPath = cmakeDir.absoluteFilePath(
                "build");  // Updated build directory outside the source
            QDir buildDir(buildDirPath);

            if (!buildDir.exists()) {
                if (!buildDir.mkpath(buildDirPath)) {
                    guiLogger.logMessage(logger::LogLevel::ERROR,
                                         "Failed to create build directory: " +
                                             buildDirPath.toStdString());
                    logOutput->append("Failed to create build directory: " +
                                      buildDirPath);
                    continue;
                }
            }

            QString exeFile = getExecutableName(buildDirPath);
            QString executablePath = buildDir.absoluteFilePath(exeFile);
            QProcess *runProcess = new QProcess(this);
            runProcess->setWorkingDirectory(buildDirPath);
            connect(runProcess, &QProcess::readyReadStandardOutput,
                    [this, runProcess]() {
                        logOutput->append(runProcess->readAllStandardOutput());
                    });
            connect(runProcess, &QProcess::readyReadStandardError,
                    [this, runProcess]() {
                        logOutput->append(runProcess->readAllStandardError());
                    });
            connect(
                runProcess,
                QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &MainWindow::processFinished);
            runProcess->start(executablePath, QStringList());
            if (!runProcess->waitForStarted()) {
                guiLogger.logMessage(logger::LogLevel::ERROR,
                                     "Failed to start the program in " +
                                         buildDirPath.toStdString());
                logOutput->append("Failed to start the program in " +
                                  buildDirPath);
                logOutput->append(runProcess->readAllStandardError());
                delete runProcess;
                continue;
            }
            runningProcesses.append(
                qMakePair(runProcess, square->getProcess()->getId()));
        }
        square->setStopButtonVisible(true);
    }

    guiLogger.logMessage(logger::LogLevel::INFO,
                         "Compiling and running projects finished.");
}

void MainWindow::editSquare(int id)
{
    guiLogger.logMessage(logger::LogLevel::INFO,
                         "Editing square with ID: " + std::to_string(id));

    for (DraggableSquare *square : squares) {
        if (square->getProcess()->getId() == id) {
            ProcessDialog dialog(this);
            dialog.setId(square->getProcess()->getId());
            dialog.setName(square->getProcess()->getName());
            dialog.setExecutionFile(square->getProcess()->getExecutionFile());
            dialog.setQEMUPlatform(square->getProcess()->getQEMUPlatform());

            if (dialog.exec() == QDialog::Accepted && dialog.isValid()) {
                // Update the process details
                // square->setProcess(Process(dialog.getId(), dialog.getName(), dialog.getCMakeProject(), dialog.getQEMUPlatform()));
                Process *updatedProcess = new Process(
                    dialog.getId(), dialog.getName(), dialog.getExecutionFile(),
                    dialog.getQEMUPlatform());
                square->setProcess(updatedProcess);
                guiLogger.logMessage(logger::LogLevel::INFO,
                                     "Updated process details for square ID: " +
                                         std::to_string(id));
            }
            break;
        }
    }
}

void MainWindow::deleteSquare(int id)
{
    guiLogger.logMessage(logger::LogLevel::INFO,
                         "Deleting square with ID: " + std::to_string(id));
    auto it = std::find_if(
        squares.begin(), squares.end(), [id](DraggableSquare *square) {
            return square && square->getProcess()->getId() == id;
        });

    if (it != squares.end()) {
        DraggableSquare *toDelete = *it;
        it = squares.erase(it);
        if (toDelete) {
            toDelete->deleteLater();
            guiLogger.logMessage(
                logger::LogLevel::INFO,
                "Square with ID: " + std::to_string(id) + " deleted.");
        }
    }
    else {
        guiLogger.logMessage(
            logger::LogLevel::ERROR,
            "Square with ID: " + std::to_string(id) + " not found.");
    }
    usedIds.remove(id);
    squarePositions.remove(id);
}

void MainWindow::createProcessConfigFile(int id, const QString &processPath)
{
    // Creating a JSON object with the process ID
    QJsonObject jsonObject;
    jsonObject["ID"] = id;

    // Converting the object to a JSON document
    QJsonDocument jsonDoc(jsonObject);

    // Defining the file name and its path
    QString filePath = processPath + "/config.json";
    QFile configFile(filePath);

    // Opening the file for writing and checking if the file opened successfully
    if (configFile.open(QIODevice::WriteOnly)) {
        configFile.write(jsonDoc.toJson());
        configFile.close();

        guiLogger.logMessage(
            logger::LogLevel::INFO,
            "Config file created at: " + filePath.toStdString());
    }
    else {
        guiLogger.logMessage(
            logger::LogLevel::ERROR,
            "Failed to create config file at: " + filePath.toStdString());
    }
}

void MainWindow::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QProcess *finishedProcess = qobject_cast<QProcess *>(sender());
    if (finishedProcess) {
        int finishedProcessId =-1;
        // Find the ID of the process that finished
        for (const QPair<QProcess *, int> &pair : runningProcesses) {
            if (pair.first == finishedProcess) {
                finishedProcessId = pair.second;
                // Find the corresponding DraggableSquare
                for (DraggableSquare *square : squares) {
                    if (square->getProcess()->getId() == finishedProcessId) {
                        square->setStopButtonVisible(false);
                        break;
                    }
                }
                break;
            }
        }
        // Remove the finished process based on ID from runningProcesses
        for (int i = 0; i < runningProcesses.size(); ++i) {
            if (runningProcesses[i].second == finishedProcessId) {
                runningProcesses.removeAt(i);  // Remove the process with the matching ID
                break;
            }
        }
        // Check if all processes have finished
        if (runningProcesses.isEmpty()) {
            hideLoadingIndicator();  // Stop spinner
            enableAllButtons();      // Re-enable buttons
        }
    }
}

// Disable all buttons except the "End" button
void MainWindow::disableButtonsExceptEnd() 
{
    compileButton->setEnabled(false);
    runButton->setEnabled(false);
    timerButton->setEnabled(false);
}

// Enable all buttons
void MainWindow::enableAllButtons() 
{
    compileButton->setEnabled(true);
    runButton->setEnabled(true);
    timerButton->setEnabled(true);
}

void MainWindow::rotateImage() {
    // Rotate the pixmap by the current angle
    QTransform transform;
    transform.rotate(rotationAngle);
    QPixmap rotatedPixmap = loadingPixmap.transformed(transform);

    // Update the label with the rotated pixmap
    loadingLabel->setPixmap(rotatedPixmap);

    // Increment the rotation angle for the next frame
    rotationAngle = (rotationAngle + 10) % 360;  // Rotate by 10 degrees each time
}

// Show the loading spinner with rotation
void MainWindow::showLoadingIndicator() {
    loadingLabel->show();
    rotationTimer->start(rotationTimerIntervals);  // Start the timer with ms intervals
}

// Hide the loading spinner and stop the rotation
void MainWindow::hideLoadingIndicator() {
    rotationTimer->stop();
    loadingLabel->hide();
}

#include "moc_main_window.cpp"