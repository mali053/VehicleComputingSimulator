#ifndef SIMULATION_DATA_MANAGER_H
#define SIMULATION_DATA_MANAGER_H

#include <QWidget>
#include <QPushButton>
#include <bson/bson.h>
#include <vector>
#include <string>

class SimulationDataManager : public QWidget {
    Q_OBJECT

public:
    SimulationDataManager(QWidget *parent = nullptr);
    virtual ~SimulationDataManager() = default;
    void save_simulation_data(const std::string &fileName);
    void load_simulation_data(const std::string &fileName);

private slots:
    void on_save_button_clicked();
    void on_load_button_clicked();

private:
    QPushButton *saveButton;
    QPushButton *loadButton;

    struct Coordinate {
        int x;
        int y;
    };

    struct Size {
        int x;
        int y;
    };

    struct Process {
        int id;
        std::string name;
        std::string code;
        std::string platform;
        Coordinate coordinate;
        Size size;
    };

    struct User {
        int id;
        std::string name;
        std::string img;
    };

    struct SimulationData {
        std::vector<Process> processes;
        User user;
    };

    SimulationData data;

    void print_json(const bson_t *document);
};

#endif // SIMULATION_DATA_MANAGER_H
