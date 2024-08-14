#include "frames.h"
#include <QTimer>
#include <cstdlib>
#include <ctime>

// Constructor to initialize Frames with a LogHandler reference
Frames::Frames(LogHandler &logHandler, QWidget *parent)
    : logHandler(logHandler), QWidget(parent), differenceTime(0)
{
    createSequentialIds();
    fillFramesMat();

    if (!logHandler.getLogEntries().isEmpty()) {
        differenceTime = logHandler.getLogEntries().first().timestamp.msecsTo(
            QDateTime::currentDateTime());
    }

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Frames::updateFrames);
    timer->start(1000);
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
                painter.setPen(QPen(QColor(frame.color), frame.thickness));

                auto square1 = logHandler.getProcessSquares()[log.srcId];
                auto square2 = logHandler.getProcessSquares()[log.dstId];

                QRect rect1(square1->getDragStartPosition().x(),
                            square1->getDragStartPosition().y(),
                            square1->width(), square1->height());
                QRect rect2(square2->getDragStartPosition().x(),
                            square2->getDragStartPosition().y(),
                            square2->width(), square2->height());
                painter.drawRect(rect1);
                painter.drawRect(rect2);
            }
            else {
                qDebug() << "Invalid index: row" << row << "col" << col;
            }
        }
        else {
            qDebug() << "Invalid ID index:";
        }
    }
}

void Frames::updateFrames()
{
    QDateTime currentTime =
        QDateTime::currentDateTime().addMSecs(-differenceTime);

    qDebug() << "Updating frames at time:" << currentTime.toString();
    // Decrease thickness for expired log entries
    for (auto it = activeLogEntries.begin(); it != activeLogEntries.end();) {
        if (it->first.addSecs(5) <= currentTime) {
            const LogHandler::LogEntry &logEntry = it->second;
            int row =
                std::max(idMapping[logEntry.dstId], idMapping[logEntry.srcId]);
            int col =
                std::min(idMapping[logEntry.dstId], idMapping[logEntry.srcId]);
            if (framesMat[row][col].thickness > 1) {
                framesMat[row][col].thickness -= 0.01;
            }
            it = activeLogEntries.erase(it);
        }
        else {
            ++it;
        }
    }

    // Increase thickness for new log entries
    for (const LogHandler::LogEntry &logEntry : logHandler.getLogEntries()) {
        if (logEntry.timestamp <= currentTime) {
            if (idMapping[logEntry.srcId] < framesMat.size() &&
                idMapping[logEntry.dstId] < framesMat.size()) {
                int row = std::max(idMapping[logEntry.dstId],
                                   idMapping[logEntry.srcId]);
                int col = std::min(idMapping[logEntry.dstId],
                                   idMapping[logEntry.srcId]);
                framesMat[row][col].thickness += 0.01;
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
        int originalId = ds->getProcess().getId();
        qDebug() << originalId;
        idMapping.insert(originalId, newId);
        newId++;
    }
    qDebug() << newId;
    initialFramesMat(newId);
}

void Frames::fillFramesMat()
{
    QSet<QString> usedColors;
    srand(static_cast<unsigned int>(time(0)));

    for (int i = 0; i < framesMat.size(); ++i) {
        for (int j = 0; j <= i; ++j) {
            QString randomColor;
            do {
                randomColor = generateRandomColor();
            } while (usedColors.contains(randomColor));

            usedColors.insert(randomColor);
            framesMat[i][j].color = randomColor;
            framesMat[i][j].thickness = 1;

            qDebug() << "Color for frame (" << i << "," << j
                     << "):" << randomColor;
        }
    }
}

QString Frames::generateRandomColor()
{
    QString color = QString("#%1%2%3")
                        .arg(rand() % 256, 2, 16, QChar('0'))
                        .arg(rand() % 256, 2, 16, QChar('0'))
                        .arg(rand() % 256, 2, 16, QChar('0'));
    return color;
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