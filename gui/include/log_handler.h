#ifndef LOGHANDLER_H
#define LOGHANDLER_H

#include "draggable_square.h"
#include <QDateTime>
#include <QDockWidget>
#include <QJsonObject>
#include <QMainWindow>
#include <QString>
#include <QTime>
#include <QVector>

class LogHandler {
public:
    struct LogEntry {
        QDateTime timestamp;
        int srcId;
        int dstId;
        QString payload;
        QString status;  // SEND/RECEIVE

        bool operator<(const LogEntry &other) const
        {
            return timestamp < other.timestamp;
        }
    };

    void readLogFile(const QString &fileName);
    void sortLogEntries();
    void analyzeLogEntries(QMainWindow *mainWindow, const QString &jsonFileName,
                           bool realTime = false);
    QVector<LogHandler::LogEntry> getLogEntries();
    const QMap<int, DraggableSquare *> &getProcessSquares() const;

private:
    QVector<LogEntry> logEntries;
    QMap<int, DraggableSquare *>
        processSquares;  // Track process squares by their IDs
    QMap<int, QMap<int, int>>
        communicationCounts;  // Track communication counts between process pairs
};

#endif  // LOGHANDLER_H