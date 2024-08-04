
#ifndef LOGHANDLER_H
#define LOGHANDLER_H

#include <QString>
#include <QVector>
#include <QTime>
#include <QDateTime>
#include <QMainWindow>
#include <QDockWidget>
#include <QJsonObject>


class LogHandler {
public:
    struct LogEntry {
        QDateTime timestamp;
        int srcId;
        int dstId;
        QString payload;
        QString status;  // SEND/RECEIVE

        bool operator<(const LogEntry& other) const {
            return timestamp < other.timestamp;
        }

    };

    void readLogFile(const QString &fileName);
    void sortLogEntries();
    void analyzeLogEntries(QMainWindow *mainWindow,const QString &fileName); 
    void draw(int xSrc, int ySrc, int xDest, int yDest);
    QVector<int> findProcessCoordinatesById(int processId, const QString &fileName);
private:
    QVector<LogEntry> logEntries;
};

#endif // LOGHANDLER_H
