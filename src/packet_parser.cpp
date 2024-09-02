#include "packet_parser.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

PacketParser::PacketParser(const std::string& jsonFilePath) {
    loadJson(jsonFilePath);
}

void PacketParser::loadJson(const std::string& jsonFilePath) {
    std::ifstream file(jsonFilePath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open JSON file");
    }

    json j;
    file >> j;
    for (const auto& field : j["fields"]) {
        fields.push_back({
            field["name"],
            field["type"],
            field["size"],
            field.value("endianness", "little")  // Default to little-endian
        });
    }
}

void PacketParser::parseBuffer(const void* buffer) {
    const uint8_t* data = static_cast<const uint8_t*>(buffer);

    // Parse the Status (7-bit unsigned integer)
    uint8_t status = data[0] & 0x7F; // Mask to get only the first 7 bits
    std::cout << "Status: " << static_cast<int>(status) << std::endl;

    // Parse the Msg (64-bit character array)
    std::string msg(reinterpret_cast<const char*>(&data[1]), 8);
    std::cout << "Msg: " << msg << std::endl;

    // Parse Temperature (32-bit float, little-endian)
    float temperature;
    std::memcpy(&temperature, &data[9], sizeof(float));
    std::cout << "Temperature: " << temperature << std::endl;

    // Parse Pressure (32-bit float with Mantissa, little-endian)
    float pressure;
    std::memcpy(&pressure, &data[13], sizeof(float));
    std::cout << "Pressure: " << pressure << std::endl;

    // Parse Flags (8-bit bit-field)
    uint8_t flags = data[17];
    std::cout << "Flags: " << static_cast<int>(flags) << std::endl;
}

