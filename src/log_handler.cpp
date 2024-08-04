#include "log_handler.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QPainter>
#include <QWidget>
#include <QTimer>
//#include "my_widget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// קריאת נתונים מקובץ
void LogHandler::readLogFile(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file:" << fileName;
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed(); // Trim the entire line
        QStringList fields = line.split(' ');

        if (fields.size() < 6) {
            qWarning() << "Skipping malformed line:" << line;
            continue;
        }

        LogEntry entry;
        QString dateString = fields[0].trimmed(); // Trim each field
        QString timeString = fields[1].trimmed();

        QString dateTimeString = dateString + " " + timeString;
        entry.timestamp = QDateTime::fromString(dateTimeString, "yyyy-MM-dd HH:mm:ss");
        if (!entry.timestamp.isValid()) {
            qWarning() << "Skipping line with invalid timestamp:" << line;
            continue;
        }

        entry.srcId = fields[2].trimmed().toInt();
        entry.dstId = fields[3].trimmed().toInt();
        entry.payload = fields[4].trimmed();
        entry.status = fields[5].trimmed();

        logEntries.append(entry);
    }

    file.close();
}


// מיון רשומות הלוג
void LogHandler::sortLogEntries() {
    std::sort(logEntries.begin(), logEntries.end());
}

void LogHandler::analyzeLogEntries(QMainWindow *mainWindow,const QString &fileName) {
//     // יצירת חלון להציג את זמן הלוג
//     QWidget *timeWidget = new QWidget(mainWindow);
//     QVBoxLayout *layout = new QVBoxLayout(timeWidget);
//     QLabel *timeLabel = new QLabel("Timestamp: ", timeWidget);
//     layout->addWidget(timeLabel);
//     timeWidget->setLayout(layout);
//     timeWidget->setWindowTitle("Current Log Timestamp");
//     timeWidget->resize(200, 100);
//     timeWidget->setAttribute(Qt::WA_QuitOnClose, false); // לוודא שהחלון לא יסגור את האפליקציה
//     timeWidget->show();

//     // יצירת חלון להציג את תרשים הזרימה
//     MyWidget *flowWidget = new MyWidget(mainWindow, 100); // קצב עדכון של 100 מ"ל
//     QDockWidget *dockWidget = new QDockWidget("Flow Diagram", mainWindow);
//     dockWidget->setWidget(flowWidget);
//     mainWindow->addDockWidget(Qt::RightDockWidgetArea, dockWidget);

//     // טיפול בכל רשומות הלוג
//     for (const auto &log_entry : logEntries) {
//         // עדכון התווית בחלון הזמן עם הת timestamp הנוכחי
//         timeLabel->setText("Timestamp: " + log_entry.timestamp.toString("yyyy-MM-dd HH:mm:ss"));
//         QCoreApplication::processEvents(); // עדכון ה-UI

//         // הוספת חץ בין הנקודות
//         QString status = log_entry.status == "sent" ? "sending" : "received";
//         // read the place from json
//         int xsrcId = 0, ysrcId = 0, xdstId = 0, ydstId = 0;
        
//         QVector<int> coordinatesSrc = findProcessCoordinatesById(log_entry.srcId,fileName);
//         if (!coordinatesSrc.isEmpty()) {
//           qDebug() << "Coordinates for process ID" << log_entry.srcId << ":" << coordinatesSrc[0] << "," << coordinatesSrc[1];
//           xsrcId=coordinatesSrc[0];
//           ysrcId=coordinatesSrc[1];
//         } else {
//             qDebug() << "Process not found or error occurred.";
//         }
//          QVector<int> coordinatesDst = findProcessCoordinatesById(log_entry.dstId,fileName);
//         if (!coordinatesDst.isEmpty()) {
//           qDebug() << "Coordinates for process ID" << log_entry.dstId << ":" << coordinatesDst[0] << "," << coordinatesDst[1];
//           xdstId=coordinatesDst[0];
//           ydstId=coordinatesDst[1];
//         } else {
//             qDebug() << "Process not found or error occurred.";
//         }
//            flowWidget->addConnection(xsrcId, ysrcId, xdstId, ydstId, log_entry.payload, status);
//     }

//     qDebug() << "Analyzing log entries";
}

QVector<int> LogHandler::findProcessCoordinatesById(int processId, const QString &fileName) {
    QVector<int> coordinates;

    // טעינת JSON מהקובץ
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Could not open file");
        return coordinates;
    }
    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument document = QJsonDocument::fromJson(jsonData);
    QJsonObject rootObject = document.object();
    QJsonArray processesArray = rootObject["processes"].toArray();

    // חיפוש ID התהליך ומציאת הקואורדינטות שלו
    for (const QJsonValue &value : processesArray) {
        QJsonObject processObject = value.toObject();
        if (processObject["id"].toInt() == processId) {
            coordinates.append(processObject["x"].toInt());
            coordinates.append(processObject["y"].toInt());
            break;
        }
    }

    return coordinates;
}
