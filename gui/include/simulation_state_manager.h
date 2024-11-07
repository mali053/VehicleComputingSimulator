#ifndef SIMULATION_STATE_MANAGER_H
#define SIMULATION_STATE_MANAGER_H

#include <QJsonObject>
#include <QPushButton>
#include <QString>
#include <QVector>
#include <QWidget>
#include <bson/bson.h>
#include "draggable_square.h"

class SimulationStateManager : public QWidget {
    Q_OBJECT

public:
    SimulationStateManager(QWidget *parent = nullptr);
    virtual ~SimulationStateManager() = default;
    void saveSimulationState(const std::string &fileName,
                             QVector<DraggableSquare *> squares, QString img);
    QJsonObject loadSimulationState(const std::string &fileName);

    void printJson(QJsonObject jsonObject);

    struct SimulationData {
        QVector<DraggableSquare *> squares;
        int id;
        QString name;
        QString img;
    };

    SimulationData data;

private:
    QJsonObject bsonToJsonObject(const bson_t *document);
    void readSimulationState(QVector<DraggableSquare *> squares, QString img);
    void updateStateFromJson(QJsonObject jsonObject);
};

#endif  // SIMULATION_STATE_MANAGER_H