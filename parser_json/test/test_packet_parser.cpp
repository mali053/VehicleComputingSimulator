#include "packet_parser.h"
#include <cstring>
#include <fstream>
#include <gtest/gtest.h>
#include <vector>

// Helper function to create a test buffer
std::vector<uint8_t> createTestBuffer() {
  std::vector<uint8_t> buffer;

  buffer.push_back(static_cast<uint8_t>(5)); // Status

  std::string msg = "Hello!";
  for (char c : msg) {
    buffer.push_back(static_cast<uint8_t>(c));
  }
  while (buffer.size() < 9) {
    buffer.push_back(0);
  }

  float temperature = 36.5f;
  uint8_t tempBytes[4];
  std::memcpy(tempBytes, &temperature, 4);
  buffer.insert(buffer.end(), tempBytes, tempBytes + 4);

  float pressure = 101.3f;
  uint8_t pressureBytes[4];
  std::memcpy(pressureBytes, &pressure, 4);
  buffer.insert(buffer.end(), pressureBytes, pressureBytes + 4);

  buffer.push_back(static_cast<uint8_t>(0xB)); // Flags

  return buffer;
}

// Helper function to create a test JSON file
void createTestJson(const std::string &filename) {
  std::ofstream jsonFile(filename);
  jsonFile << R"(
    {
        "endianness": "big",
        "fields": [
            {"name": "status", "type": "unsigned_int", "size": 8},
            {"name": "message", "type": "char_array", "size": 64},
            {"name": "temperature", "type": "float_fixed", "size": 32},
            {"name": "pressure", "type": "float_fixed", "size": 32},
            {"name": "flags", "type": "unsigned_int", "size": 8}
        ]
    }
    )";
  jsonFile.close();
}

// Test that parses the buffer correctly
TEST(PacketParserTest, ParseBuffer) {
  std::vector<uint8_t> buffer = createTestBuffer();
  std::string jsonFilePath = "test.json";
  createTestJson(jsonFilePath);

  PacketParser parser(jsonFilePath, buffer.data());

  EXPECT_EQ(*static_cast<uint32_t *>(parser.getFieldValue("status")), 5);
  EXPECT_EQ(*static_cast<std::string *>(parser.getFieldValue("message")),
            "Hello!");
  EXPECT_FLOAT_EQ(*static_cast<float *>(parser.getFieldValue("temperature")),
                  36.5f);
  EXPECT_FLOAT_EQ(*static_cast<float *>(parser.getFieldValue("pressure")),
                  101.3f);
  EXPECT_EQ(*static_cast<uint32_t *>(parser.getFieldValue("flags")), 0xB);

  // Clean up
  std::remove(jsonFilePath.c_str());
}

// Test that field type detection works
TEST(PacketParserTest, FieldTypeDetection) {
  PacketParser parser("", nullptr); // No buffer needed for this test

  EXPECT_EQ(parser.getFieldType("unsigned_int"), FieldType::UNSIGNED_INT);
  EXPECT_EQ(parser.getFieldType("char_array"), FieldType::CHAR_ARRAY);
  EXPECT_EQ(parser.getFieldType("float_fixed"), FieldType::FLOAT_FIXED);
  EXPECT_EQ(parser.getFieldType("float_mantissa"), FieldType::FLOAT_MANTISSA);
  EXPECT_EQ(parser.getFieldType("bit_field"), FieldType::BIT_FIELD);
  EXPECT_EQ(parser.getFieldType("double"), FieldType::DOUBLE);
  EXPECT_EQ(parser.getFieldType("signed_int"), FieldType::SIGNED_INT);
  EXPECT_EQ(parser.getFieldType("unknown"), FieldType::UNKNOWN);
}

// Test that invalid field names throw an error
TEST(PacketParserTest, InvalidFieldName) {
  std::vector<uint8_t> buffer = createTestBuffer();
  std::string jsonFilePath = "test.json";
  createTestJson(jsonFilePath);

  PacketParser parser(jsonFilePath, buffer.data());

  EXPECT_THROW(parser.getFieldValue("invalid_field"), std::runtime_error);

  // Clean up
  std::remove(jsonFilePath.c_str());
}

// Test that invalid JSON file throws an error
TEST(PacketParserTest, InvalidJsonFile) {
  std::vector<uint8_t> buffer = createTestBuffer();
  std::string invalidJsonFilePath = "invalid_test.json";

  EXPECT_THROW(PacketParser parser(invalidJsonFilePath, buffer.data()),
               std::runtime_error);

  // Clean up
  std::remove(invalidJsonFilePath.c_str());
}

// Test that buffer overflow is handled correctly
TEST(PacketParserTest, BufferOverflow) {
  std::vector<uint8_t> buffer = createTestBuffer();
  std::string jsonFilePath = "test.json";
  createTestJson(jsonFilePath);

  PacketParser parser(jsonFilePath, buffer.data());

  // Modify buffer to be too small
  std::vector<uint8_t> smallBuffer(
      buffer.begin(), buffer.begin() + 8); // Remove part of the buffer
  EXPECT_THROW(parser.getFieldValue("message"),
               std::out_of_range); // Should throw due to buffer overflow

  // Clean up
  std::remove(jsonFilePath.c_str());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
