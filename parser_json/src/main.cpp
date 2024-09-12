#include "packet_parser.h"
#include <bitset>
#include <cstring>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;

// פונקציה להדפסת הבאפר בפורמט בינארי
void printBufferBinary(const std::vector<uint8_t> &buffer) {
  std::cout << "Buffer (Binary): ";
  for (const auto &byte : buffer) {
    std::bitset<8> bs(byte);
    std::cout << bs << " ";
  }
  std::cout << std::endl;
}

int main() {
  try {
    std::string jsonFilePath;
    std::cout << "Enter the path to the JSON file: ";
    std::cin >> jsonFilePath;

    // Construct the buffer
    std::vector<uint8_t> buffer;

    // CharArrayField: 64-bit character array (8 bytes)
    std::string charArrayField = "HelloWorld!";
    for (char c : charArrayField) {
      buffer.push_back(static_cast<uint8_t>(c));
    }
    while (buffer.size() < 8) { // Fill the remaining space
      buffer.push_back(0);
    }

    // FloatField: 32-bit float, little-endian
    float floatField = 3.14f;
    uint8_t floatFieldBytes[4];
    std::memcpy(floatFieldBytes, &floatField, 4);
    for (int i = 0; i < 4; i++) {
      buffer.push_back(floatFieldBytes[i]);
    }

    // DoubleField: 64-bit double, little-endian
    double doubleField = 6.28;
    uint8_t doubleFieldBytes[8];
    std::memcpy(doubleFieldBytes, &doubleField, 8);
    for (int i = 0; i < 8; i++) {
      buffer.push_back(doubleFieldBytes[i]);
    }

    // SignedIntField: 32-bit signed integer, little-endian
    int32_t signedIntField = -12345;
    uint8_t signedIntFieldBytes[4];
    std::memcpy(signedIntFieldBytes, &signedIntField, 4);
    for (int i = 0; i < 4; i++) {
      buffer.push_back(signedIntFieldBytes[i]);
    }

    // UnsignedIntField: 16-bit unsigned integer, little-endian
    uint16_t unsignedIntField = 54321;
    uint8_t unsignedIntFieldBytes[2];
    std::memcpy(unsignedIntFieldBytes, &unsignedIntField, 2);
    for (int i = 0; i < 2; i++) {
      buffer.push_back(unsignedIntFieldBytes[i]);
    }

    // FloatMantissaField: 32-bit float, little-endian
    float floatMantissaField = 1.2345f;
    uint8_t floatMantissaFieldBytes[4];
    std::memcpy(floatMantissaFieldBytes, &floatMantissaField, 4);
    for (int i = 0; i < 4; i++) {
      buffer.push_back(floatMantissaFieldBytes[i]);
    }

    // GenericBitField: 32-bit bit field
    uint8_t field1 = 0x5; // 3 bits (0b101)
    uint8_t field2 = 0x1; // 2 bits (0b01)
    uint8_t field3 = 0x3; // 3 bits (0b011)

    // uint8_t bitField = (field1 & 0x7) | ((field2 & 0x3) << 3) | ((field3 &
    // 0x7) << 5);
    uint8_t bitField =
        ((field3 & 0x7) << 5) | ((field2 & 0x3) << 3) | (field1 & 0x7);

    // // הוספת הביטפילד לבופר
    buffer.push_back(bitField);

    // ExtraField1: 8-bit unsigned integer
    uint8_t extraField1 = 0x12;
    buffer.push_back(extraField1);

    // ExtraField2: 128-bit character array (16 bytes)
    std::string extraField2 = "ExtraDataField";
    for (char c : extraField2) {
      buffer.push_back(static_cast<uint8_t>(c));
    }

    size_t padding = 16 - extraField2.size();
    for (size_t i = 0; i < padding; ++i) {
      buffer.push_back(0);
    }

    // NewSignedIntField: 16-bit signed integer, little-endian
    int16_t newSignedIntField = -32000;
    uint8_t newSignedIntFieldBytes[2];
    std::memcpy(newSignedIntFieldBytes, &newSignedIntField, 2);
    for (int i = 0; i < 2; i++) {
      buffer.push_back(newSignedIntFieldBytes[i]);
    }



    PacketParser parser(jsonFilePath, buffer.data());
    
    parser.getAllFieldValues();

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  return 0;
}