#ifndef FRAMES_H
#define FRAMES_H

#include <cstdlib>
#include <ctime>
#include <vector>
#include <QDateTime>
#include <QHash>
#include <QMap>
#include <QSet>
#include <QString>
#include "log_handler.h"

class Frames {
public:
    struct Frame {
        QString color;
        int thickness;
    };
    LogHandler logHandler;
    std::vector<std::vector<Frame>> framesMat;
    std::vector<LogHandler::LogEntry> activeLogEntriesVector;
    QHash<int, int> idMapping;
    QDateTime currentTime;
    Frames(LogHandler &logHandler);
  
    void updateFrames(const QDateTime &currentTime);
    void fillFramesMat();
    // Getters
    const LogHandler& getLogHandler() const;
    const std::vector<std::vector<Frame>>& getFramesMat() const; 
    const std::vector<LogHandler::LogEntry>& getLogEntriesVector() const;
    const QHash<int, int> getIdMapping() const;
    const QDateTime getCurrentTime() const;
    // Setters
    void setLogHandler(LogHandler &logHandler);
    void setFramesMat(const std::vector<std::vector<Frame>>& framesMat);
    void setLogEntriesVector(const std::vector<LogHandler::LogEntry>& logEntriesVector);
    void setIdMapping(QHash<int, int> &idMapping);
    void setCurrentTime(const QDateTime &currentTime);
private:
    void initialFramesMat(int size);
    void createSequentialIds();
    QString generateRandomColor();
};

#endif // FRAMES_H