#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QPainter>
#include <QTextStream>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include "log_handler.h"
#include "draggable_square.h"
#include "simulation_state_manager.h"
#include "main_window.h"

QVector<LogHandler::LogEntry> LogHandler::getLogEntries()
{
    return logEntries;
}

void LogHandler::readLogFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        MainWindow::guiLogger.logMessage(
            logger::LogLevel::ERROR,
            "Cannot open file: " + fileName.toStdString());
        return;
    }

    MainWindow::guiLogger.logMessage(
        logger::LogLevel::INFO,
        "File successfully opened: " + fileName.toStdString());

    QByteArray fileData = file.readAll();
    file.close();

    QList<QByteArray> lines = fileData.split('\n');
    for (const QByteArray &line : lines) {
        QString trimmedLine = QString(line).trimmed();
        QStringList fields = trimmedLine.split(' ');

        if (fields.size() < 6) {
            MainWindow::guiLogger.logMessage(
                logger::LogLevel::DEBUG,
                "Skipping malformed line: " + trimmedLine.toStdString());
            continue;
        }

        LogEntry entry;
        QString dateString = fields[0].trimmed();
        QString timeString = fields[1].trimmed();

        QString dateTimeString = dateString + " " + timeString;
        entry.timestamp =
            QDateTime::fromString(dateTimeString, "yyyy-MM-dd HH:mm:ss.zzz");
        if (!entry.timestamp.isValid()) {
            MainWindow::guiLogger.logMessage(
                logger::LogLevel::ERROR,
                "Skipping line with invalid timestamp: " +
                    trimmedLine.toStdString());
            continue;
        }

        entry.srcId = fields[2].trimmed().toInt();
        entry.dstId = fields[3].trimmed().toInt();
        entry.payload = fields[4].trimmed();
        entry.status = fields[5].trimmed();
        logEntries.push_back(entry);
    }

    MainWindow::guiLogger.logMessage(logger::LogLevel::INFO,
                                     "Log file successfully read.");
}

void LogHandler::sortLogEntries()
{
    std::sort(logEntries.begin(), logEntries.end());
}

void LogHandler::analyzeLogEntries(QMainWindow *mainWindow,
                                   const QString &jsonFileName, bool realTime)
{
    if (realTime) {
        // if the simulation runs time the squares are present on the window
        /// Otherwise, quarters are reloaded according to the data in Gison
        MainWindow::guiLogger.logMessage(logger::LogLevel::INFO,
                                         "Analyzing log entries in real-time.");
    }
    else {
        MainWindow::guiLogger.logMessage(
            logger::LogLevel::INFO, "Analyzing log entries from JSON file: " +
                                        jsonFileName.toStdString());

        SimulationStateManager stateManager;
        QJsonObject jsonObject =
            stateManager.loadSimulationState(jsonFileName.toStdString());
        if (jsonObject.isEmpty()) {
            MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR,
                                             "Failed to load JSON data");
            return;
        }

        MainWindow::guiLogger.logMessage(
            logger::LogLevel::INFO,
            "Successfully loaded simulation data from: " +
                jsonFileName.toStdString());

        // Update process map with JSON data
        QJsonArray processesArray = jsonObject["squares"].toArray();
        for (const QJsonValue &value : processesArray) {
            QJsonObject processObject = value.toObject();
            int id = processObject["id"].toInt();
            QString name = processObject["name"].toString();
            QString cmakeProject = processObject["CMakeProject"].toString();
            QString qemuPlatform = processObject["QEMUPlatform"].toString();
            int x = processObject["position"].toObject()["x"].toInt();
            int y = processObject["position"].toObject()["y"].toInt();
            int width = processObject["width"].toInt();
            int height = processObject["height"].toInt();

            Process *process =
                new Process(id, name, cmakeProject, qemuPlatform);
            DraggableSquare *square =
                new DraggableSquare(mainWindow, "", width, height);
            square->setProcess(process);
            square->move(x, y);
            processSquares.insert(id, square);
        }
    }

    MainWindow::guiLogger.logMessage(
        logger::LogLevel::INFO,
        "Size of logEntries: " + std::to_string(logEntries.size()));

    for (const auto &logEntry : logEntries) {
        int srcId = logEntry.srcId;
        int dstId = logEntry.dstId;

        if (!processSquares.contains(srcId) || !processSquares.contains(dstId))
            continue;

        DraggableSquare *srcSquare = processSquares[srcId];
        DraggableSquare *dstSquare = processSquares[dstId];

        QString color = "background-color: yellow;";  // Default color
        if (communicationCounts.contains(srcId) &&
            communicationCounts[srcId].contains(dstId)) {
            int count = communicationCounts[srcId][dstId];
            int colorIntensity = qMin(
                count * 25, 255);  // Increase color intensity based on count
            color = QString("background-color: rgb(%1, %1, 255);")
                        .arg(colorIntensity);
        }

        srcSquare->setSquareColor(color);
        dstSquare->setSquareColor(color);

        MainWindow::guiLogger.logMessage(
            logger::LogLevel::DEBUG,
            "Updated square colors for srcId: " + std::to_string(srcId) +
                " dstId: " + std::to_string(dstId));

        // Increase communication count
        communicationCounts[srcId][dstId]++;
    }
    for (QMap<int, DraggableSquare *>::iterator it = processSquares.begin();
         it != processSquares.end(); ++it) {
        // Access key and value
        int key = it.key();
        DraggableSquare *square = it.value();
        square
            ->print();  // For example, a call to the print function we defined
    }
}

const QMap<int, DraggableSquare *> &LogHandler::getProcessSquares() const
{
    return processSquares;
}