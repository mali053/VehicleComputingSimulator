#include <QTimer>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "frames.h"
#include "main_window.h"

//Setting constants
const int timerIntervalMs = 100;
const int squareOffset = 8;
const int cornerRadius = 10;
const double minThickness = 0.001;
const double maxThickness = 25;
const int logEntryExpirySecs = 5;
const int alphaValue = 12;

// Constructor to initialize Frames with a LogHandler reference
Frames::Frames(LogHandler &logHandler, QWidget *parent)
    : logHandler(logHandler), QWidget(parent), differenceTime(0)
{
    MainWindow::guiLogger.logMessage(logger::LogLevel::INFO,
                                     "Initializing Frames");

    createSequentialIds();
    fillFramesMat();

    if (!logHandler.getLogEntries().isEmpty()) {
        differenceTime = logHandler.getLogEntries().first().timestamp.msecsTo(
            QDateTime::currentDateTime());
    }

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Frames::updateFrames);
    timer->start(timerIntervalMs);

    MainWindow::guiLogger.logMessage(
        logger::LogLevel::DEBUG, "Frames initialized with timer set to 100ms");
}

void Frames::initialFramesMat(int size)
{
    framesMat.resize(size);
    for (int i = 0; i < size; ++i) {
        framesMat[i].resize(i + 1);  // Resize each row to i+1 elements
    }
}

void Frames::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    for (const auto &log : logHandler.getLogEntries()) {
        int dstId = idMapping[log.dstId];
        int srcId = idMapping[log.srcId];

        if (dstId != -1 && srcId != -1) {
            int row = std::max(dstId, srcId);
            int col = std::min(dstId, srcId);

            if (row < framesMat.size() && col < framesMat[row].size()) {
                const Frame &frame = framesMat[row][col];

                QPen pen(QColor(frame.color), frame.thickness);
                pen.setJoinStyle(Qt::RoundJoin);
                painter.setPen(pen);

                auto square1 = logHandler.getProcessSquares()[log.srcId];
                auto square2 = logHandler.getProcessSquares()[log.dstId];

                QRect rect1(square1->pos().x() - squareOffset, square1->pos().y() - squareOffset,
                            square1->width(), square1->height());
                QRect rect2(square2->pos().x() - squareOffset, square2->pos().y() - squareOffset,
                            square2->width(), square2->height());

                painter.drawRoundedRect(rect1, cornerRadius, cornerRadius);
                painter.drawRoundedRect(rect2, cornerRadius, cornerRadius);
            }
            else {
                MainWindow::guiLogger.logMessage(
                    logger::LogLevel::ERROR, "Invalid index: row " +
                                                 std::to_string(row) + " col " +
                                                 std::to_string(col));
            }
        }
        else {
            MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR,
                                             "Invalid ID index");
        }
    }
}

void Frames::updateFrames()
{
    QDateTime currentTime =
        QDateTime::currentDateTime().addMSecs(-differenceTime);

    MainWindow::guiLogger.logMessage(
        logger::LogLevel::DEBUG,
        "Updating frames at time: " + currentTime.toString().toStdString());

    // Decrease thickness for expired log entries
    for (auto it = activeLogEntries.begin(); it != activeLogEntries.end();) {
        if (it->first.addSecs(logEntryExpirySecs) <= currentTime) {
            const LogHandler::LogEntry &logEntry = it->second;
            int row =
                std::max(idMapping[logEntry.dstId], idMapping[logEntry.srcId]);
            int col =
                std::min(idMapping[logEntry.dstId], idMapping[logEntry.srcId]);

            framesMat[row][col].thickness -= minThickness;
            if (framesMat[row][col].thickness <= minThickness)
                it = activeLogEntries.erase(it);
        }
        else {
            ++it;
        }
    }

    MainWindow::guiLogger.logMessage(
        logger::LogLevel::INFO, "Active log entries remaining: " +
                                    std::to_string(activeLogEntries.size()));

    // Increase thickness for new log entries
    for (const LogHandler::LogEntry &logEntry : logHandler.getLogEntries()) {
        if (logEntry.timestamp <= currentTime) {
            if (idMapping[logEntry.srcId] < framesMat.size() &&
                idMapping[logEntry.dstId] < framesMat.size()) {
                int row = std::max(idMapping[logEntry.dstId],
                                   idMapping[logEntry.srcId]);
                int col = std::min(idMapping[logEntry.dstId],
                                   idMapping[logEntry.srcId]);
                if(framesMat[row][col].thickness <= maxThickness)
                    framesMat[row][col].thickness += minThickness;
                activeLogEntries.emplace(logEntry.timestamp, logEntry);
            }
        }
    }
    update();
}

void Frames::createSequentialIds()
{
    int newId = 0;
    for (const DraggableSquare *ds : logHandler.getProcessSquares()) {
        int originalId = ds->getProcess()->getId();
        MainWindow::guiLogger.logMessage(
            logger::LogLevel::DEBUG, "Mapping original ID " +
                                         std::to_string(originalId) +
                                         " to new ID " + std::to_string(newId));
        idMapping.insert(originalId, newId);
        newId++;
    }
    initialFramesMat(newId);
}

void Frames::fillFramesMat()
{
    QSet<QString> usedColors;
    srand(static_cast<unsigned int>(time(0)));

    for (int i = 0; i < framesMat.size(); ++i) {
        for (int j = 0; j < i; ++j) {
            QString randomColor;
            do {
                randomColor = generateRandomColor();
            } while (usedColors.contains(randomColor));

            usedColors.insert(randomColor);
            framesMat[i][j].color = randomColor;
            framesMat[i][j].thickness = 1;

            MainWindow::guiLogger.logMessage(
                logger::LogLevel::DEBUG,
                "Assigned color " + randomColor.toStdString() + " to frame (" +
                    std::to_string(i) + ", " + std::to_string(j) + ")");
        }
    }
}

QString Frames::generateRandomColor()
{
    QString color = QString("#%1%2%3%4")
                        .arg(rand() % 256, 2, 16, QChar('0'))
                        .arg(rand() % 256, 2, 16, QChar('0'))
                        .arg(rand() % 256, 2, 16, QChar('0'))
                        .arg(alphaValue, 2, 16, QChar('0'));
    return color.toUpper();
}

// Getters
const LogHandler &Frames::getLogHandler() const
{
    return logHandler;
}

const std::vector<std::vector<Frames::Frame>> &Frames::getFramesMat() const
{
    return framesMat;
}

const std::multimap<QDateTime, LogHandler::LogEntry>
    &Frames::getActiveLogEntries() const
{
    return activeLogEntries;
}

QMap<int, int> Frames::getIdMapping() const
{
    return idMapping;
}

// Setters
void Frames::setLogHandler(LogHandler &logHandler)
{
    this->logHandler = logHandler;
}

void Frames::setFramesMat(const std::vector<std::vector<Frame>> &framesMat)
{
    this->framesMat = framesMat;
}

void Frames::setActiveLogEntries(
    const std::multimap<QDateTime, LogHandler::LogEntry> &logEntriesVector)
{
    this->activeLogEntries = logEntriesVector;
}

void Frames::setIdMapping(const QMap<int, int> &idMapping)
{
    this->idMapping = idMapping;
}