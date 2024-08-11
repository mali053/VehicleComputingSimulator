#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QTextStream>
#include <QTimer>
#include <QWidget>
#include <QCoreApplication>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QVBoxLayout>
#include "draggable_square.h"
#include "simulation_data_manager.h"
#include "log_handler.h"


QVector<LogHandler::LogEntry> LogHandler::getLogEntries() { return logEntries; }
// Reading data from a file
void LogHandler::readLogFile(const QString &fileName) {
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Cannot open file:" << fileName;
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
    entry.timestamp =
        QDateTime::fromString(dateTimeString, "yyyy-MM-dd HH:mm:ss");
    if (!entry.timestamp.isValid()) {
      qWarning() << "Skipping line with invalid timestamp:" << line;
      continue;
    }

    entry.srcId = fields[2].trimmed().toInt();
    entry.dstId = fields[3].trimmed().toInt();
    entry.payload = fields[4].trimmed();
    entry.status = fields[5].trimmed();
    logEntries.push_back(entry);
  }

  file.close();
  qDebug() << "succesfull read LogFile";
}

void LogHandler::sortLogEntries() {
  std::sort(logEntries.begin(), logEntries.end());
}

void LogHandler::analyzeLogEntries(QMainWindow *mainWindow,
                                   const QString &jsonFileName, bool realTime) {
  if (realTime) {
    // if the simulation runs time the squares are present on the window
    /// Otherwise, quarters are reloaded according to the data in Gison
  } else {
    SimulationDataManager dataManager;
    QJsonObject jsonObject =
        dataManager.loadSimulationData(jsonFileName.toStdString());
    if (jsonObject.isEmpty()) {
      qWarning() << "Failed to load JSON data";
      return;
    }

    // עדכון מפת התהליכים לפי מידע מהקובץ JSON
    QJsonArray processesArray = jsonObject["processes"].toArray();
    for (const QJsonValue &value : processesArray) {
      QJsonObject processObject = value.toObject();
      int id = processObject["id"].toInt();
      QString name = processObject["name"].toString();
      QString cmakeProject = processObject["CMakeProject"].toString();
      QString qemuPlatform = processObject["QEMUPlatform"].toString();
      int x = processObject["coordinate"].toObject()["x"].toInt();
      int y = processObject["coordinate"].toObject()["y"].toInt();
      int width = processObject["width"].toInt();
      int height = processObject["height"].toInt();

      Process process(id, name, cmakeProject, qemuPlatform);
      DraggableSquare *square =
          new DraggableSquare(mainWindow, "", width, height);
      square->setProcess(process);
      square->move(x, y);
      processSquares.insert(id, square);
    }
  }

  qDebug() << "Size of logEntries:" << logEntries.size();
  for (const auto &logEntry : logEntries) {
    int srcId = logEntry.srcId;
    int dstId = logEntry.dstId;

    if (!processSquares.contains(srcId) || !processSquares.contains(dstId))
      continue;

    DraggableSquare *srcSquare = processSquares[srcId];
    DraggableSquare *dstSquare = processSquares[dstId];

    QString color = "background-color: yellow;"; // Default color
    if (communicationCounts.contains(srcId) &&
        communicationCounts[srcId].contains(dstId)) {
      int count = communicationCounts[srcId][dstId];
      int colorIntensity =
          qMin(count * 25, 255); // Increase color intensity based on count
      color =
          QString("background-color: rgb(%1, %1, 255);").arg(colorIntensity);
    }

    srcSquare->setSquareColor(color);
    dstSquare->setSquareColor(color);

    // Increase communication count
    communicationCounts[srcId][dstId]++;
  }
  for (QMap<int, DraggableSquare *>::iterator it = processSquares.begin();
       it != processSquares.end(); ++it) {
    // Access key and value
    int key = it.key();
    DraggableSquare *square = it.value();
    square->print(); // For example, a call to the print function we defined
  }
}

QVector<int> LogHandler::findProcessCoordinatesById(int processId, const QString &fileName) {
  QVector<int> coordinates;

  // Load JSON from file
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

  // Searching for the process ID and finding its coordinatesFi
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

const QMap<int, DraggableSquare*>& LogHandler::getProcessSquares() const { return processSquares; }