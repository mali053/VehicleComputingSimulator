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
#include "simulation_data_manager.h"

SimulationDataManager::SimulationDataManager(QWidget *parent) : QWidget(parent)
{
}

void SimulationDataManager::readSimulationData(QVector<DraggableSquare*> squares, QString img)
{
    data.processes.clear();
    for(int i = 0; i < squares.size(); i++) {
        if(squares[i] != nullptr)
            data.processes.append(squares[i]);
        else
            qWarning() << "Warning: Null pointer at index" << i;
    }

    data.user.id = 1;
    data.user.name = "default user";
    data.user.img = img;
}

void SimulationDataManager::saveSimulationData(
    const std::string &fileName, QVector<DraggableSquare *> squares,
    QString img)
{
    readSimulationData(squares, img);

    bson_t *document = bson_new();

    bson_t processes;
    BSON_APPEND_ARRAY_BEGIN(document, "processes", &processes);
    for (const auto &process : data.processes) {
        bson_t proc;
        char key[16];
        snprintf(key, sizeof(key), "%d", process->getProcess().getId());
        BSON_APPEND_DOCUMENT_BEGIN(&processes, key, &proc);
        BSON_APPEND_INT32(&proc, "id", process->getProcess().getId());
        BSON_APPEND_UTF8(&proc, "name",
                         process->getProcess().getName().toStdString().c_str());
        BSON_APPEND_UTF8(
            &proc, "CMakeProject",
            process->getProcess().getCMakeProject().toStdString().c_str());
        BSON_APPEND_UTF8(
            &proc, "QEMUPlatform",
            process->getProcess().getQEMUPlatform().toStdString().c_str());

        bson_t coordinate;
        BSON_APPEND_DOCUMENT_BEGIN(&proc, "coordinate", &coordinate);
        BSON_APPEND_INT32(&coordinate, "x",
                          process->getDragStartPosition().x());
        BSON_APPEND_INT32(&coordinate, "y",
                          process->getDragStartPosition().y());
        bson_append_document_end(&proc, &coordinate);

        BSON_APPEND_INT32(&proc, "width", process->width());
        BSON_APPEND_INT32(&proc, "height", process->height());

        bson_append_document_end(&processes, &proc);
    }
    bson_append_array_end(document, &processes);

    bson_t user;
    BSON_APPEND_DOCUMENT_BEGIN(document, "user", &user);
    BSON_APPEND_INT32(&user, "id", data.user.id);
    BSON_APPEND_UTF8(&user, "name", data.user.name.toStdString().c_str());
    BSON_APPEND_UTF8(&user, "img", data.user.img.toStdString().c_str());
    bson_append_document_end(document, &user);

    uint32_t length;
    uint8_t *buf = bson_destroy_with_steal(document, true, &length);

    std::ofstream file(fileName, std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<char *>(buf), length);
        file.close();
        std::cout << "Successfully saved data to " << fileName << std::endl;
    }
    else {
        std::cerr << "Failed to open file for writing: " << fileName
                  << std::endl;
    }

    bson_free(buf);
}

QJsonObject SimulationDataManager::loadSimulationData(
    const std::string &fileName)
{
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
            QJsonObject jsonObject = bsonToJsonObject(document);
            bson_destroy(document);  // Clean up BSON document
            return jsonObject;
        }
        else {
            std::cerr << "Failed to parse BSON document" << std::endl;
        }
    }
    else {
        std::cerr << "Failed to read file: " << fileName << std::endl;
    }
    return QJsonObject();  // Return an empty QJsonObject
}

QJsonObject SimulationDataManager::bsonToJsonObject(const bson_t *document)
{
    char *json = bson_as_json(document, nullptr);
    QJsonDocument jsonDoc =
        QJsonDocument::fromJson(QByteArray::fromRawData(json, strlen(json)));
    bson_free(json);
    return jsonDoc.object();
}

void SimulationDataManager::printJson(QJsonObject jsonObject)
{
    QJsonDocument jsonDoc(jsonObject);
    QByteArray jsonBytes = jsonDoc.toJson();
    std::cout << jsonBytes.toStdString() << std::endl;
}