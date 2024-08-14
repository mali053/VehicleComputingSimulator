#ifndef __FRAMES_H__
#define __FRAMES_H__

#include "log_handler.h"
#include <QDateTime>
#include <QHash>
#include <QMap>
#include <QPainter>
#include <QPen>
#include <QSet>
#include <QString>
#include <vector>

class Frames : public QWidget {
    Q_OBJECT

public:
    struct Frame {
        QString color;
        double thickness;
    };

    // Ctor
    Frames(LogHandler &logHandler, QWidget *parent = nullptr);

    // Getters
    const LogHandler &getLogHandler() const;
    const std::vector<std::vector<Frame>> &getFramesMat() const;
    const std::multimap<QDateTime, LogHandler::LogEntry> &getActiveLogEntries()
        const;
    QMap<int, int> getIdMapping() const;

    // Setters
    void setLogHandler(LogHandler &logHandler);
    void setFramesMat(const std::vector<std::vector<Frame>> &framesMat);
    void setActiveLogEntries(
        const std::multimap<QDateTime, LogHandler::LogEntry> &logEntriesVector);
    void setIdMapping(const QMap<int, int> &idMapping);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateFrames();

private:
    LogHandler &logHandler;
    std::vector<std::vector<Frame>> framesMat;
    std::multimap<QDateTime, LogHandler::LogEntry> activeLogEntries;
    QMap<int, int> idMapping;
    qint64 differenceTime;

    void initialFramesMat(int size);
    void createSequentialIds();
    QString generateRandomColor();
    void fillFramesMat();
};

#endif  // __FRAMES_H__