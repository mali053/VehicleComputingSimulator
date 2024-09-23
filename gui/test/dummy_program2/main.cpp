#include <chrono>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <thread>
#include <chrono>

using json = nlohmann::json;

int main() 
{
    // Read JSON file
    std::ifstream file("../config.json");  // Replace with your JSON file name
    if (!file.is_open()) {
        std::cerr << "Unable to open file!" << std::endl;
        return 1;
    }

    json jsonObject;
    file >> jsonObject;

    // Extract ID from JSON
    int id = jsonObject["ID"];  // Assuming the JSON structure has an "id" field
    std::cout << "Loaded ID from JSON: " << id << std::endl;

    // Simulate work
    for (int i = 0; i < 10; ++i) {
        std::cout << "Dummy Program 2 is running: " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));  // Simulate work
    }

    return 0;
}