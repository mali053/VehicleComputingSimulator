#include <QFile>
#include <QJsonArray>
#include <QJsonValue>
#include <fstream>
#include <iostream>
#include <QVBoxLayout>
#include <QPushButton>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include "main_window.h"
#include "simulation_state_manager.h"
#include "main_window.h"

SimulationStateManager::SimulationStateManager(QWidget *parent)
    : QWidget(parent)
{
    MainWindow::guiLogger.logMessage(logger::LogLevel::INFO,
                                     "SimulationStateManager", "Constructor",
                                     "SimulationStateManager instance created");
}

void SimulationStateManager::readSimulationState(
    QVector<DraggableSquare *> squares, QString img)
{
    MainWindow::guiLogger.logMessage(
        logger::LogLevel::DEBUG, "SimulationStateManager",
        "readSimulationState", "Starting to read simulation state");
    data.squares.clear();
    MainWindow::guiLogger.logMessage(
        logger::LogLevel::DEBUG, "SimulationStateManager",
        "readSimulationState", "Cleared existing processes");
    for (int i = 0; i < squares.size(); i++) {
        if (squares[i] != nullptr) {
            data.squares.append(squares[i]);
            MainWindow::guiLogger.logMessage(
                logger::LogLevel::DEBUG, "SimulationStateManager",
                "readSimulationState",
                "Added process at index " + std::to_string(i));
        }
        else
            MainWindow::guiLogger.logMessage(
                logger::LogLevel::ERROR, "SimulationStateManager",
                "readSimulationState",
                "Null pointer encountered at index " + std::to_string(i));
    }

    data.id = 1;
    data.name = "default name";
    data.img = img;

    MainWindow::guiLogger.logMessage(
        logger::LogLevel::INFO, "SimulationStateManager", "readSimulationState",
        "Simulation state read completed. Total processes: " +
            std::to_string(data.squares.size()));
}

void SimulationStateManager::saveSimulationState(
    const std::string &fileName, QVector<DraggableSquare *> squares,
    QString img)
{
    MainWindow::guiLogger.logMessage(logger::LogLevel::INFO,
                                     "Starting to save simulation state.");
    readSimulationState(squares, img);

    bson_t *document = bson_new();

    bson_t squaresBson;
    BSON_APPEND_ARRAY_BEGIN(document, "squares", &squaresBson);
    for (const auto &square : data.squares) {
        bson_t squareBson;
        char key[16];
        snprintf(key, sizeof(key), "%d", square->getProcess()->getId());
        BSON_APPEND_DOCUMENT_BEGIN(&squaresBson, key, &squareBson);
        BSON_APPEND_INT32(&squareBson, "id", square->getProcess()->getId());
        BSON_APPEND_UTF8(&squareBson, "name",
                         square->getProcess()->getName().toStdString().c_str());
        BSON_APPEND_UTF8(
            &squareBson, "CMakeProject",
            square->getProcess()->getExecutionFile().toStdString().c_str());
        BSON_APPEND_UTF8(
            &squareBson, "QEMUPlatform",
            square->getProcess()->getQEMUPlatform().toStdString().c_str());

        bson_t position;
        BSON_APPEND_DOCUMENT_BEGIN(&squareBson, "position", &position);
        BSON_APPEND_INT32(&position, "x", square->pos().x());
        BSON_APPEND_INT32(&position, "y", square->pos().y());
        bson_append_document_end(&squareBson, &position);

        BSON_APPEND_INT32(&squareBson, "width", square->width());
        BSON_APPEND_INT32(&squareBson, "height", square->height());
        BSON_APPEND_UTF8(&squareBson, "color",
                         square->styleSheet().toStdString().c_str());

        bson_append_document_end(&squaresBson, &squareBson);
    }
    bson_append_array_end(document, &squaresBson);

    BSON_APPEND_INT32(document, "id", data.id);
    BSON_APPEND_UTF8(document, "name", data.name.toStdString().c_str());
    BSON_APPEND_UTF8(document, "img", data.img.toStdString().c_str());

    uint32_t length;
    uint8_t *buf = bson_destroy_with_steal(document, true, &length);

    std::ofstream file(fileName, std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<char *>(buf), length);
        file.close();
        MainWindow::guiLogger.logMessage(
            logger::LogLevel::INFO, "Successfully saved state to " + fileName);
    }
    else {
        MainWindow::guiLogger.logMessage(
            logger::LogLevel::ERROR,
            "Failed to open file for writing: " + fileName);
    }

    bson_free(buf);
}

QJsonObject SimulationStateManager::loadSimulationState(
    const std::string &fileName)
{
    MainWindow::guiLogger.logMessage(
        logger::LogLevel::INFO,
        "Attempting to load simulation state from " + fileName);
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return QJsonObject();  // Return an empty QJsonObject
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size)) {
        const uint8_t *data = reinterpret_cast<const uint8_t *>(buffer.data());
        bson_t *document = bson_new_from_data(data, size);
        if (document) {
            MainWindow::guiLogger.logMessage(
                logger::LogLevel::INFO,
                "Successfully loaded simulation state.");
            QJsonObject jsonObject = bsonToJsonObject(document);
            bson_destroy(document);  // Clean up BSON document
            printJson(jsonObject);
            updateStateFromJson(jsonObject);
            return jsonObject;
        }
        else {
            MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR,
                                             "Failed to parse BSON document");
        }
    }
    else {
        MainWindow::guiLogger.logMessage(logger::LogLevel::ERROR,
                                         "Failed to read file: " + fileName);
    }
    return QJsonObject();  // Return an empty QJsonObject
}

void SimulationStateManager::updateStateFromJson(QJsonObject jsonObject)
{
    data.id = jsonObject["id"].toInt();
    data.name = jsonObject["name"].toString();
    data.img = jsonObject["img"].toString();

    // Update squares data
    QJsonArray squaresArray = jsonObject["squares"].toArray();
    data.squares.clear();
    for (int i = 0; i < squaresArray.size(); ++i) {
        QJsonObject squareObj = squaresArray[i].toObject();

        // Create a new DraggableSquare object dynamically
        int id = squareObj["id"].toInt();
        QString name = squareObj["name"].toString();
        QString CMakeProject = squareObj["CMakeProject"].toString();
        QString QEMUPlatform = squareObj["QEMUPlatform"].toString();

        QJsonObject positionObj = squareObj["position"].toObject();
        int x = positionObj["x"].toInt();
        int y = positionObj["y"].toInt();

        int width = squareObj["width"].toInt();
        int height = squareObj["height"].toInt();

        // Load color
        QString color = squareObj["color"].toString();

        // Allocate a new DraggableSquare on the heap
        DraggableSquare *square =
            new DraggableSquare(nullptr, color, width, height);
        square->setProcess(new Process(id, name, CMakeProject, QEMUPlatform));
        square->move(QPoint(x, y));
        // Add the pointer to the vector
        data.squares.append(square);
    }
}

QJsonObject SimulationStateManager::bsonToJsonObject(const bson_t *document)
{
    char *json = bson_as_json(document, nullptr);
    QJsonDocument jsonDoc =
        QJsonDocument::fromJson(QByteArray::fromRawData(json, strlen(json)));
    MainWindow::guiLogger.logMessage(logger::LogLevel::INFO,
                                     "Converted BSON to JSON object.");
    bson_free(json);
    return jsonDoc.object();
}

void SimulationStateManager::printJson(QJsonObject jsonObject)
{
    QJsonDocument jsonDoc(jsonObject);
    QByteArray jsonBytes = jsonDoc.toJson();
    MainWindow::guiLogger.logMessage(
        logger::LogLevel::DEBUG,
        "Printing JSON object:\n" + jsonBytes.toStdString());
}