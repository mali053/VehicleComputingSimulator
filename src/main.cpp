#include "packet_parser.h"
#include <iostream>
#include <vector>
#include <cstring>

int main() {
    try {
        std::string jsonFilePath;
        std::cout << "Enter the path to the JSON file: ";
        std::cin >> jsonFilePath;
        PacketParser parser(jsonFilePath);

        //PacketParser parser("packet_structure.json");


        // Construct the buffer
        std::vector<uint8_t> buffer;

        // Status: 7-bit unsigned integer with value 5
        buffer.push_back(static_cast<uint8_t>(5)); // 5 in binary is 00000101

        // Msg: 64-bit character array with the string "Hello!"
        std::string msg = "Hello!";
        for (char c : msg) {
            buffer.push_back(static_cast<uint8_t>(c));
        }
        while (buffer.size() < 9) { // Fill the remaining space
            buffer.push_back(0);
        }

        // Temperature: 32-bit float, little-endian
        float temperature = 36.5f;
        uint8_t tempBytes[4];
        std::memcpy(tempBytes, &temperature, 4);
        for (int i = 0; i < 4; i++) {
            buffer.push_back(tempBytes[i]);
        }

        // Pressure: 32-bit float, little-endian
        float pressure = 101.3f;
        uint8_t pressureBytes[4];
        std::memcpy(pressureBytes, &pressure, 4);
        for (int i = 0; i < 4; i++) {
            buffer.push_back(pressureBytes[i]);
        }

        // Flags: 8-bit bit-field, example value 0xB (1011 in binary)
        buffer.push_back(static_cast<uint8_t>(0xB));

        // Parse the buffer
        parser.parseBuffer(buffer.data());



    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
