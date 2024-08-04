#include "simulation_data_manager.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <fstream>
#include <iostream>

SimulationDataManager::SimulationDataManager(QWidget *parent) : QWidget(parent) 
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    saveButton = new QPushButton("Save Simulation Data", this);
    loadButton = new QPushButton("Load Simulation Data", this);
    layout->addWidget(saveButton);
    layout->addWidget(loadButton);

    connect(saveButton, &QPushButton::clicked, this, &SimulationDataManager::on_save_button_clicked);
    connect(loadButton, &QPushButton::clicked, this, &SimulationDataManager::on_load_button_clicked);

    // Initialize simulation data (for demonstration)
    data.processes = { {1, "Process1", "Code1", "Platform1", {10, 20}, {5, 5}},
                       {2, "Process2", "Code2", "Platform2", {20, 30}, {5, 5}}};
    data.user = { 1, "User1", "./img" };
}

void SimulationDataManager::on_save_button_clicked() 
{
    save_simulation_data("simulation_data.bson");
}

void SimulationDataManager::on_load_button_clicked() 
{
    load_simulation_data("simulation_data.bson");
}

void SimulationDataManager::save_simulation_data(const std::string &fileName)
{
    bson_t *document = bson_new();

    bson_t processes;
    BSON_APPEND_ARRAY_BEGIN(document, "processes", &processes);
    for (const auto &process : data.processes) {
        bson_t proc;
        char key[16];
        snprintf(key, sizeof(key), "%d", process.id);
        BSON_APPEND_DOCUMENT_BEGIN(&processes, key, &proc);
        BSON_APPEND_INT32(&proc, "id", process.id);
        BSON_APPEND_UTF8(&proc, "name", process.name.c_str());
        BSON_APPEND_UTF8(&proc, "code", process.code.c_str());
        BSON_APPEND_UTF8(&proc, "platform", process.platform.c_str());

        bson_t coordinate;
        BSON_APPEND_DOCUMENT_BEGIN(&proc, "coordinate", &coordinate);
        BSON_APPEND_INT32(&coordinate, "x", process.coordinate.x);
        BSON_APPEND_INT32(&coordinate, "y", process.coordinate.y);
        bson_append_document_end(&proc, &coordinate);

        bson_t size;
        BSON_APPEND_DOCUMENT_BEGIN(&proc, "size", &size);
        BSON_APPEND_INT32(&size, "x", process.size.x);
        BSON_APPEND_INT32(&size, "y", process.size.y);
        bson_append_document_end(&proc, &size);

        bson_append_document_end(&processes, &proc);
    }
    bson_append_array_end(document, &processes);

    bson_t user;
    BSON_APPEND_DOCUMENT_BEGIN(document, "user", &user);
    BSON_APPEND_INT32(&user, "id", data.user.id);
    BSON_APPEND_UTF8(&user, "name", data.user.name.c_str());
    BSON_APPEND_UTF8(&user, "img", data.user.img.c_str());
    bson_append_document_end(document, &user);

    // Convert BSON to JSON and print
    print_json(document);

    uint32_t length;
    uint8_t *buf = bson_destroy_with_steal(document, true, &length);

    std::ofstream file(fileName, std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<char *>(buf), length);
        file.close();
        std::cout << "Successfully saved data to " << fileName << std::endl;
    } else {
        std::cerr << "Failed to open file for writing: " << fileName << std::endl;
    }

    bson_free(buf);
}

void SimulationDataManager::load_simulation_data(const std::string &fileName)
{
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << fileName << std::endl;
        return;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size)) {
        const uint8_t *data = reinterpret_cast<const uint8_t *>(buffer.data());
        bson_t *document = bson_new_from_data(data, size);
        if (document) {
            print_json(document);
            bson_destroy(document);
        } else {
            std::cerr << "Failed to parse BSON document" << std::endl;
        }
    } else {
        std::cerr << "Failed to read file: " << fileName << std::endl;
    }
}

void SimulationDataManager::print_json(const bson_t *document) 
{
    char *json = bson_as_json(document, nullptr);
    std::cout << json << std::endl;
    bson_free(json);
}
