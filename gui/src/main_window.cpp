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

int sizeSquare = 120;
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

    timeLabel->hide();
    timeInput->hide();
    logOutput->setReadOnly(true);

    mainLayout->addWidget(toolbox);

    QPushButton *addProcessButton = new QPushButton("Add Process", toolbox);
    toolboxLayout->addWidget(addProcessButton);
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

    toolbox->setMaximumWidth(100);
    toolbox->setMinimumWidth(100);
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
        new Process(id, "Main", "../src/dummy_program1", "QEMUPlatform");
    addProcessSquare(mainProcess, id, styleSheet);
    addId(id++);
    Process *hsmProcess =
        new Process(id, "HSM", "../src/dummy_program2", "QEMUPlatform");
    addProcessSquare(hsmProcess, id, styleSheet);
    addId(id++);
    Process *logsDbProcess =
        new Process(id, "LogsDb", "../src/dummy_program1", "QEMUPlatform");
    addProcessSquare(logsDbProcess, id, styleSheet);
    addId(id++);
    Process *busManagerProcess =
        new Process(id, "Bus_Manager", "../src/dummy_program2", "QEMUPlatform");
    addProcessSquare(busManagerProcess, id, styleSheet);
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
            new Process(id, dialog.getName(), dialog.getCMakeProject(),
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

    createProcessConfigFile(process->getId(), process->getCMakeProject());
}

void MainWindow::addProcessSquare(Process *&process, int index,
                                  const QString &color)
{
    DraggableSquare *square =
        new DraggableSquare(workspace, color, sizeSquare, sizeSquare);

    square->setProcess(process);
    square->setId(process->getId());
    int x = (index % 2) * (square->width() + 10);
    int y = (index / 2) * (square->height() + 10);
    QPoint pos = squarePositions.value(process->getId(), QPoint(x, y));
    square->move(pos);
    square->show();

    squarePositions[process->getId()] = pos;
    squares.push_back(square);

    createProcessConfigFile(process->getId(), process->getCMakeProject());
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
        connect(timer, &QTimer::timeout, this, &MainWindow::timerTimeout);

        timer->start(time * 1000);
        timeLabel->hide();
        timeInput->hide();
    }
}

void MainWindow::endProcesses()
{
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
                                     "data saved to simulation_data.bson");

    QString filePath = "../log_file.log";
    logHandler.readLogFile(filePath);
    logHandler.analyzeLogEntries(this, "simulation_data.bson");

    frames = new Frames(logHandler);  // Initialize Frames
    QVBoxLayout *framesLayout = new QVBoxLayout(workspace);
    framesLayout->addWidget(frames);
    workspace->setLayout(framesLayout);

    for (const QPair<QProcess *, int> &pair : runningProcesses) {
        QProcess *process = pair.first;
        int id = pair.second;
        if (process->state() != QProcess::NotRunning) {
            process->terminate();
            process->waitForFinished();
        }

        delete process;
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
            runningProcesses.removeAt(i);
            break;
        }
    }
}

void MainWindow::showTimerInput()
{
    timeLabel->show();
    timeInput->show();

    guiLogger.logMessage(
        logger::LogLevel::DEBUG,
        "showTimerInput() called: Timer input elements are shown.");
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
    updateTimer();

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

    for (DraggableSquare *square : squares) {
        QString cmakePath = square->getProcess()->getCMakeProject();

        if (cmakePath.endsWith(".sh")) {
            // Shell script processing
            QFile scriptFile(cmakePath);
            if (!scriptFile.exists()) {
                guiLogger.logMessage(logger::LogLevel::ERROR,
                                     "Shell script file does not exist: " +
                                         cmakePath.toStdString());

                logOutput->append("Shell script file does not exist: " +
                                  cmakePath);
                compileSuccessful = false;
                continue;
            }

            if ((scriptFile.permissions() & QFileDevice::ExeUser) == 0) {
                // Make the script executable
                QProcess makeExecutableProcess;
                makeExecutableProcess.start("chmod", QStringList()
                                                         << "+x" << cmakePath);
                if (!makeExecutableProcess.waitForFinished()) {
                    logOutput->append("Failed to make the script executable: " +
                                      cmakePath);
                    compileSuccessful = false;
                    continue;
                }
                guiLogger.logMessage(
                    logger::LogLevel::INFO,
                    "Script is now executable: " + cmakePath.toStdString());
                logOutput->append("Script is now executable: " + cmakePath);
            }

            // Run the shell script
            QProcess *scriptProcess = new QProcess(this);
            scriptProcess->start("bash", QStringList() << cmakePath);
            connect(
                scriptProcess,
                QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &MainWindow::processFinished);
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
            guiLogger.logMessage(logger::LogLevel::INFO,
                                 "Compiling " + cmakePath.toStdString());

            // CMake project processing
            logOutput->append("Compiling " + cmakePath);
            QDir cmakeDir(cmakePath);
            QString buildDirPath = cmakeDir.absoluteFilePath("build");
            QDir buildDir(buildDirPath);

            if (buildDir.exists()) {
                QFileInfoList fileList = buildDir.entryInfoList(
                    QDir::NoDotAndDotDot | QDir::AllEntries);
                foreach (const QFileInfo &fileInfo, fileList) {
                    if (fileInfo.isDir()) {
                        QDir dir(fileInfo.absoluteFilePath());
                        if (!dir.removeRecursively()) {
                            guiLogger.logMessage(
                                logger::LogLevel::ERROR,
                                "Failed to remove directory: " +
                                    fileInfo.absoluteFilePath().toStdString());

                            logOutput->append("Failed to remove directory: " +
                                              fileInfo.absoluteFilePath());
                            compileSuccessful = false;
                            continue;
                        }
                    }
                    else {
                        if (!QFile::remove(fileInfo.absoluteFilePath())) {
                            guiLogger.logMessage(
                                logger::LogLevel::ERROR,
                                "Failed to remove file: " +
                                    fileInfo.absoluteFilePath().toStdString());
                            logOutput->append("Failed to remove file: " +
                                              fileInfo.absoluteFilePath());
                            compileSuccessful = false;
                            continue;
                        }
                    }
                }
            }
            else {
                // Create the build directory
                if (!buildDir.mkpath(".")) {
                    guiLogger.logMessage(logger::LogLevel::ERROR,
                                         "Failed to create build directory " +
                                             buildDirPath.toStdString());
                    logOutput->append("Failed to create build directory " +
                                      buildDirPath);
                    compileSuccessful = false;
                    continue;
                }
            }

            // Run CMake
            QProcess *cmakeProcess = new QProcess(this);
            cmakeProcess->setWorkingDirectory(buildDirPath);
            cmakeProcess->start("cmake", QStringList() << "..");
            connect(
                cmakeProcess,
                QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &MainWindow::processFinished);

            if (!cmakeProcess->waitForFinished()) {
                guiLogger.logMessage(
                    logger::LogLevel::ERROR,
                    "Failed to run cmake in " + buildDirPath.toStdString());
                logOutput->append("Failed to run cmake in " + buildDirPath);
                logOutput->append(cmakeProcess->readAllStandardError());
                delete cmakeProcess;
                compileSuccessful = false;
                continue;
            }
            logOutput->append(cmakeProcess->readAllStandardOutput());
            logOutput->append(cmakeProcess->readAllStandardError());
            delete cmakeProcess;

            // Run make
            QProcess *makeProcess = new QProcess(this);
            makeProcess->setWorkingDirectory(buildDirPath);
            makeProcess->start("make", QStringList());
            connect(
                makeProcess,
                QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &MainWindow::processFinished);

            if (!makeProcess->waitForFinished()) {
                guiLogger.logMessage(
                    logger::LogLevel::ERROR,
                    "Failed to compile in " + buildDirPath.toStdString());
                logOutput->append("Failed to compile in " + buildDirPath);
                logOutput->append(makeProcess->readAllStandardError());
                delete makeProcess;
                compileSuccessful = false;
                continue;
            }
            logOutput->append(makeProcess->readAllStandardOutput());
            logOutput->append(makeProcess->readAllStandardError());
            guiLogger.logMessage(
                logger::LogLevel::INFO,
                "Successfully compiled " + buildDirPath.toStdString());
            logOutput->append("Successfully compiled " + buildDirPath);
            delete makeProcess;
        }
    }

    // Enable the run button only if the compilation was successful
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
    updateTimer();
    for (DraggableSquare *square : squares) {
        QString cmakePath = square->getProcess()->getCMakeProject();

        if (cmakePath.endsWith(".sh")) {
            // Run the shell script
            QProcess *scriptProcess = new QProcess(this);
            scriptProcess->start("bash", QStringList() << cmakePath);
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
            // Run the compiled program
            QDir cmakeDir(cmakePath);
            QString buildDirPath = cmakeDir.absoluteFilePath("build");
            QDir buildDir(buildDirPath);
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
            dialog.setCMakeProject(square->getProcess()->getCMakeProject());
            dialog.setQEMUPlatform(square->getProcess()->getQEMUPlatform());

            if (dialog.exec() == QDialog::Accepted && dialog.isValid()) {
                // Update the process details
                // square->setProcess(Process(dialog.getId(), dialog.getName(), dialog.getCMakeProject(), dialog.getQEMUPlatform()));
                Process *updatedProcess = new Process(
                    dialog.getId(), dialog.getName(), dialog.getCMakeProject(),
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
        // Find the ID of the process that finished
        for (const QPair<QProcess *, int> &pair : runningProcesses) {
            if (pair.first == finishedProcess) {
                int finishedProcessId = pair.second;
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
    }
}

#include "moc_main_window.cpp"