#include <vector>
#include <QDateTime>
#include <QHash>
#include "frames.h"

// Constructor to initialize Frames with a LogHandler reference
Frames::Frames(LogHandler& logHandler) 
    : logHandler(logHandler) {  // Initialize the logHandler member variable directly
    createSequentialIds();
    fillFramesMat(); 
    // Initialize currentTime with the first log entry's timestamp, if available
    if (!logHandler.getLogEntries().isEmpty()) {
        currentTime = logHandler.getLogEntries().first().timestamp;
    } else {
        // Handle case where there are no log entries
        currentTime = QDateTime::currentDateTime(); // Default to current time
    }
}

void Frames::initialFramesMat(int size) {
    framesMat.resize(size);
    for (int i = 0; i < size; ++i) {
        framesMat[i].resize(i + 1);  // Resize each row to i+1 elements
    }
}

// Update frames with logs within a 5-second window around currentTime
void Frames::updateFrames(const QDateTime &currentTime) {
    // Delete logs that have gone beyond the 5 second range
    auto it = std::remove_if(activeLogEntriesVector.begin(), activeLogEntriesVector.end(),
                             [&](const LogHandler::LogEntry &entry) {
                                 return entry.timestamp < currentTime.addSecs(-5);
                             });
    activeLogEntriesVector.erase(it, activeLogEntriesVector.end());

    // Add new logs within the 5-second range
    const auto &allLogEntries = logHandler.getLogEntries();
    for (const auto &entry : allLogEntries) {
        if (entry.timestamp >= currentTime && entry.timestamp < currentTime.addSecs(5)) {
            activeLogEntriesVector.push_back(entry);
        }
    }
}

// Create a mapping of original IDs to new sequential IDs
void Frames::createSequentialIds() {
    int newId = 0;
    for (auto it = logHandler.getProcessSquares().begin();
         it != logHandler.getProcessSquares().end(); ++it) {
        int originalId = it.key();
        idMapping.insert(originalId, newId);
        ++newId;
    }
    initialFramesMat(newId);
}

// Fill the frames matrix with random colors
void Frames::fillFramesMat() {
    QSet<QString> usedColors;
    srand(static_cast<unsigned int>(time(0))); // Seed for random color generation

    for (int i = 0; i < framesMat.size(); ++i) {
        for (int j = 0; j <= i; ++j) { // Fill only the lower triangle (including diagonal)
            QString randomColor;
            do {
                randomColor = generateRandomColor();
            } while (usedColors.contains(randomColor));

            usedColors.insert(randomColor);
            framesMat[i][j].color = randomColor;
        }
    }
}

// Generate a random color in hexadecimal format
QString Frames::generateRandomColor() {
    QString color = QString("#%1%2%3")
                      .arg(rand() % 256, 2, 16, QChar('0'))
                      .arg(rand() % 256, 2, 16, QChar('0'))
                      .arg(rand() % 256, 2, 16, QChar('0'));
    return color;
}

// Implementation of getters
const LogHandler& Frames::getLogHandler() const {
    return logHandler;
}

const std::vector<std::vector<Frames::Frame>>& Frames::getFramesMat() const {
    return framesMat;
}

const std::vector<LogHandler::LogEntry>& Frames::getLogEntriesVector() const {
    return activeLogEntriesVector;
}

const QHash<int, int> Frames::getIdMapping() const {
    return idMapping;
}

const QDateTime Frames::getCurrentTime() const {
    return currentTime;
}

// Setters
void Frames::setLogHandler(LogHandler &logHandler) {
    this->logHandler = logHandler;
    // Optionally update other fields based on the new logHandler if needed
}

void Frames::setFramesMat(const std::vector<std::vector<Frame>>& framesMat) {
    this->framesMat = framesMat;
}

void Frames::setLogEntriesVector(const std::vector<LogHandler::LogEntry>& logEntriesVector) {
    this->activeLogEntriesVector = logEntriesVector;
}

void Frames::setIdMapping(QHash<int, int> &idMapping) {
    this->idMapping = idMapping;
}

void Frames::setCurrentTime(const QDateTime &currentTime) {
    this->currentTime = currentTime;
}