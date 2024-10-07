#include <cstring>
#include <fstream>
#include <gtest/gtest.h>
#include <vector>
#include <chrono>
#include <sstream>
#include <iostream>
#include <bitset>
#include <filesystem>
#include "packet_parser.h"

class PacketParserTest : public PacketParser {
public:
    PacketParserTest(const std::string &jsonFilePath) : PacketParser(jsonFilePath) {}

    using PacketParser::extractBits;
};

// Helper function to create a test buffer
std::vector<uint8_t> createTestBuffer()
{
    std::vector<uint8_t> buffer;

    // Status (32 bits, unsigned)
    uint32_t status = 5;
    buffer.push_back(
        static_cast<uint8_t>(status >> 24));  // Most significant byte
    buffer.push_back(static_cast<uint8_t>(status >> 16));
    buffer.push_back(static_cast<uint8_t>(status >> 8));
    buffer.push_back(static_cast<uint8_t>(status));  // Least significant byte

    // Message (64 bits, char array)
    std::string msg = "Hello!";
    for (char c : msg) {
        buffer.push_back(static_cast<uint8_t>(c));
    }
    // Pad remaining bytes to make 64 bits (8 bytes total)
    while (buffer.size() < 12) {  // 4 bytes from status + 8 bytes for message
        buffer.push_back(0);
    }

    // Temperature (32 bits, float, big-endian)
    float temperature = 36.5f;
    uint8_t tempBytes[4];
    std::memcpy(tempBytes, &temperature, sizeof(float));
    buffer.push_back(tempBytes[3]);  // Big-endian (most significant byte first)
    buffer.push_back(tempBytes[2]);
    buffer.push_back(tempBytes[1]);
    buffer.push_back(tempBytes[0]);  // Least significant byte

    // Pressure (32 bits, float, big-endian)
    float pressure = 101.3f;
    uint8_t pressBytes[4];
    std::memcpy(pressBytes, &pressure, sizeof(float));
    buffer.push_back(
        pressBytes[3]);  // Big-endian (most significant byte first)
    buffer.push_back(pressBytes[2]);
    buffer.push_back(pressBytes[1]);
    buffer.push_back(pressBytes[0]);  // Least significant byte

    // Flags (32 bits, signed integer)
    int32_t flags = -5;
    buffer.push_back(
        static_cast<uint8_t>(flags >> 24));  // Most significant byte
    buffer.push_back(static_cast<uint8_t>(flags >> 16));
    buffer.push_back(static_cast<uint8_t>(flags >> 8));
    buffer.push_back(static_cast<uint8_t>(flags));  // Least significant byte

    // Adding a double value in big-endian format (64 bits)
    double doubleValue = 12345.6789;
    uint8_t doubleBytes[8];  // 8 bytes for double
    std::memcpy(doubleBytes, &doubleValue, sizeof(double));

    // Since we need to convert to big-endian, we add the bytes from most significant to least significant
    buffer.push_back(doubleBytes[7]);  // Most significant byte
    buffer.push_back(doubleBytes[6]);
    buffer.push_back(doubleBytes[5]);
    buffer.push_back(doubleBytes[4]);
    buffer.push_back(doubleBytes[3]);
    buffer.push_back(doubleBytes[2]);
    buffer.push_back(doubleBytes[1]);
    buffer.push_back(doubleBytes[0]);  // Least significant byte

    return buffer;
}

// Helper function to create a test buffer based on the provided schema
std::vector<uint8_t> createTestCommunicationBuffer()
{
    std::vector<uint8_t> buffer;

    // Create AlertDetails (8 bits total)
    uint8_t messageType = 0x0;  // 1 bit (0b1)
    uint8_t level = 0x3;        // 3 bits (0b011)
    uint8_t objectType = 0x4;   // 4 bits (0b0100)

    // Pack the bits into 8 bits
    uint8_t bitField = (messageType & 0x01) |       // MessageType in bits 0-0
                       ((level & 0x07) << 1) |      // Level in bits 1-3
                       ((objectType & 0x0F) << 4);  // ObjectType in bits 4-7

    buffer.push_back(bitField);

    // ObjectDistance (32 bits, float, little-endian)
    float objectDistance = 123.456f;
    uint8_t objDistBytes[4];
    std::memcpy(objDistBytes, &objectDistance, sizeof(objectDistance));
    buffer.insert(buffer.end(), objDistBytes, objDistBytes + 4);

    // CarSpeed (32 bits, unsigned int, little-endian)
    uint32_t carSpeed = 60;
    buffer.insert(buffer.end(), reinterpret_cast<uint8_t *>(&carSpeed),
                  reinterpret_cast<uint8_t *>(&carSpeed) + 4);

    // ObjectSpeed (32 bits, unsigned int, little-endian)
    uint32_t objectSpeed = 80;
    buffer.insert(buffer.end(), reinterpret_cast<uint8_t *>(&objectSpeed),
                  reinterpret_cast<uint8_t *>(&objectSpeed) + 4);

    return buffer;
}

// Helper function to generate a unique filename
std::string generateUniqueJsonFileName()
{
    static int fileCounter = 0;  // Simple counter to generate unique filenames
    std::ostringstream filename;
    filename << "../test/json_test_" << fileCounter++ << ".json";
    return filename.str();
}

// Helper function to create a test JSON file for little endian data
void createLittleEndianJson(const std::string &filename)
{
    std::ofstream jsonFile(filename);
    jsonFile << R"(
    {
        "endianness": "little",
        "fields": [
            {
                "name": "status",
                "type": "unsigned_int",
                "size": 32
            },
            {
                "name": "message",
                "type": "char_array",
                "size": 64
            },
            {
                "name": "temperature",
                "type": "float_fixed",
                "size": 32
            },
            {
                "name": "pressure",
                "type": "float_fixed",
                "size": 32
            },
            {
                "name": "flags",
                "type": "signed_int",
                "size": 32
            },
            {
                "name": "double_value",
                "type": "double",
                "size": 64
            }
        ]
    }
    )";
    jsonFile.close();
}

// Helper function to create a test JSON file for little endian data with bit_field
void createLittleEndianBitFieldJson(const std::string &filename)
{
    std::ofstream jsonFile(filename);
    jsonFile << R"(
    {
        "endianness": "little",
        "fields": [
            {
                "name": "status",
                "type": "unsigned_int",
                "size": 32
            },
            {
                "name": "message",
                "type": "char_array",
                "size": 64
            },
            {
                "name": "temperature",
                "type": "float_fixed",
                "size": 32
            },
            {
                "name": "pressure",
                "type": "float_fixed",
                "size": 32
            },
            {
                "name": "flags",
                "type": "signed_int",
                "size": 32
            },
            {
                "name": "double_value",
                "type": "double",
                "size": 64
            },
            {
                "name": "bit_field",
                "type": "bit_field",
                "size": 32,
                "fields": [
                    {
                        "name": "sub_field_1",
                        "type": "unsigned_int",
                        "size": 8
                    },
                    {
                        "name": "sub_field_2",
                        "type": "unsigned_int",
                        "size": 8
                    },
                    {
                        "name": "sub_field_3",
                        "type": "unsigned_int",
                        "size": 16
                    }
                ]
            }
        ]
    })";
    jsonFile.close();
}

// Helper function to create a JSON file based on the provided schema
void createTestCommunicationJson(const std::string &filename)
{
    std::ofstream jsonFile(filename);
    jsonFile << R"(
    {
        "endianness": "little",
        "fields": [
            {
                "name": "AlertDetails",
                "type": "bit_field",
                "size": 8,
                "fields": [
                    {
                        "name": "MessageType",
                        "type": "unsigned_int",
                        "size": 1
                    },
                    {
                        "name": "Level",
                        "type": "unsigned_int",
                        "size": 3
                    },
                    {
                        "name": "ObjectType",
                        "type": "unsigned_int",
                        "size": 4
                    }
                ]
            },
            {
                "name": "ObjectDistance",
                "type": "float_fixed",
                "size": 32
            },
            {
                "name": "CarSpeed",
                "type": "unsigned_int",
                "size": 32
            },
            {
                "name": "ObjectSpeed",
                "type": "unsigned_int",
                "size": 32
            }
        ]
    }
    )";
    jsonFile.close();
}

// Helper function to create a test JSON file for bit_field with nested fields of int and char
void createBitFieldJson(const std::string &filename)
{
    std::ofstream jsonFile(filename);
    jsonFile << R"(
    {
        "endianness": "big",
        "fields": [
            {
                "name": "bit_field_example",
                "type": "bit_field",
                "size": 64,
                "fields": [
                    {
                        "name": "int_field",
                        "type": "signed_int",
                        "size": 32
                    },
                    {
                        "name": "char_array_field",
                        "type": "char_array",
                        "size": 32
                    }
                ]
            }
        ]
    }
    )";
    jsonFile.close();
}

// Helper function to create a test JSON file for ExtractBits
void createExtractBitsJson(const std::string &filename)
{
    std::ofstream jsonFile(filename);
    jsonFile << R"(
    {
        "endianness": "big",
        "fields": [
            {
                "name": "bit_field_example",
                "type": "bit_field",
                "size": 64,
                "fields": [
                    {
                        "name": "field1",
                        "type": "unsigned_int",
                        "size": 8
                    },
                    {
                        "name": "field2",
                        "type": "unsigned_int",
                        "size": 8
                    },
                    {
                        "name": "field3",
                        "type": "unsigned_int",
                        "size": 16
                    }
                ]
            }
        ]
    }
    )";
    jsonFile.close();
}

// Helper function to create a test JSON file
void createTestJson(const std::string &filename)
{
    std::ofstream jsonFile(filename);
    jsonFile << R"(
    {
        "endianness": "big",
        "fields": [
            {
                "name": "status",
                "type": "unsigned_int",
                "size": 32
            },
            {
                "name": "message",
                "type": "char_array",
                "size": 64
            },
            {
                "name": "temperature",
                "type": "float_mantissa",
                "size": 32
            },
            {
                "name": "pressure",
                "type": "float_fixed",
                "size": 32
            },
            {
                "name": "flags",
                "type": "signed_int",
                "size": 32
            },
            {
                "name": "double_value",
                "type": "double",
                "size": 64
            }
        ]
    }
    )";
    jsonFile.close();
}

// Helper function to create a test JSON file with default values
void createJsonWithDefaultValues(const std::string &filename)
{
    std::ofstream jsonFile(filename);
    jsonFile << R"(
    {
        "endianness": "big",
        "fields": [
            {
                "name": "unsignedIntField",
                "type": "unsigned_int",
                "size": 32,
                "default_value": 100
            },
            {
                "name": "intField",
                "type": "signed_int",
                "size": 32,
                "default_value": -50
            },
            {
                "name": "stringField",
                "type": "char_array",
                "size": 64
            },
            {
                "name": "floatField",
                "type": "float_fixed",
                "size": 32,
                "default_value": 3.14
            },
            {
                "name": "doubleField",
                "type": "double",
                "size": 64,
                "default_value": 2.718
            },
            {
                "name": "boolField",
                "type": "boolean",
                "size": 8,
                "default_value": true
            }
        ]
    }
    )";
    jsonFile.close();
}

// Helper function to clean up all generated JSON files
void cleanupJsonFile(const std::string &filename)
{
    if (std::filesystem::exists(filename)) {
        std::filesystem::remove(filename);
    }
}

// Create a buffer with little-endian data
std::vector<uint8_t> createLittleEndianBuffer()
{
    std::vector<uint8_t> buffer;

    // Status (32 bits, unsigned, little-endian)
    uint32_t status = 10;
    buffer.push_back(static_cast<uint8_t>(status));
    buffer.push_back(static_cast<uint8_t>(status >> 8));
    buffer.push_back(static_cast<uint8_t>(status >> 16));
    buffer.push_back(static_cast<uint8_t>(status >> 24));

    // Message (64 bits, char array)
    std::string msg = "Hello!";
    for (char c : msg) {
        buffer.push_back(static_cast<uint8_t>(c));
    }
    // Pad remaining bytes to make 64 bits (8 bytes total)
    while (buffer.size() < 12) {
        buffer.push_back(0);
    }

    // Temperature (32 bits, float, little-endian)
    float temperature = 25.0f;
    uint8_t tempBytes[4];
    std::memcpy(tempBytes, &temperature, sizeof(float));
    buffer.insert(buffer.end(), tempBytes, tempBytes + 4);

    // Pressure (32 bits, float, little-endian)
    float pressure = 50.0f;
    uint8_t pressBytes[4];
    std::memcpy(pressBytes, &pressure, sizeof(float));
    buffer.insert(buffer.end(), pressBytes, pressBytes + 4);

    // Flags (32 bits, signed integer, little-endian)
    int32_t flags = 15;
    uint8_t flagsBytes[4];
    std::memcpy(flagsBytes, &flags, sizeof(int32_t));
    buffer.insert(buffer.end(), flagsBytes, flagsBytes + 4);

    // Double value (64 bits, little-endian)
    double doubleValue = 54321.9876;
    uint8_t doubleBytes[8];
    std::memcpy(doubleBytes, &doubleValue, sizeof(double));
    buffer.insert(buffer.end(), doubleBytes, doubleBytes + 8);

    return buffer;
}

// Create a buffer with little-endian data according to the JSON schema
std::vector<uint8_t> createLittleEndianTestBuffer()
{
    std::vector<uint8_t> buffer;

    // Status (32 bits, unsigned, little-endian)
    uint32_t status = 10;
    buffer.push_back(static_cast<uint8_t>(status & 0xFF));        // Byte 0
    buffer.push_back(static_cast<uint8_t>((status >> 8) & 0xFF)); // Byte 1
    buffer.push_back(static_cast<uint8_t>((status >> 16) & 0xFF)); // Byte 2
    buffer.push_back(static_cast<uint8_t>((status >> 24) & 0xFF)); // Byte 3

    // Message (64 bits, char array)
    std::string msg = "Hello!";
    for (char c : msg) {
        buffer.push_back(static_cast<uint8_t>(c));
    }
    // Pad remaining bytes to make 64 bits (8 bytes total)
    while (buffer.size() < 12) {
        buffer.push_back(0); // Pad with null bytes
    }

    // Temperature (32 bits, float, little-endian)
    float temperature = 25.0f;
    uint8_t tempBytes[4];
    std::memcpy(tempBytes, &temperature, sizeof(float));
    buffer.insert(buffer.end(), tempBytes, tempBytes + 4);

    // Pressure (32 bits, float, little-endian)
    float pressure = 50.0f;
    uint8_t pressBytes[4];
    std::memcpy(pressBytes, &pressure, sizeof(float));
    buffer.insert(buffer.end(), pressBytes, pressBytes + 4);

    // Flags (32 bits, signed integer, little-endian)
    int32_t flags = 15;
    uint8_t flagsBytes[4];
    std::memcpy(flagsBytes, &flags, sizeof(int32_t));
    buffer.insert(buffer.end(), flagsBytes, flagsBytes + 4);

    // Double value (64 bits, little-endian)
    double doubleValue = 54321.9876;
    uint8_t doubleBytes[8];
    std::memcpy(doubleBytes, &doubleValue, sizeof(double));
    buffer.insert(buffer.end(), doubleBytes, doubleBytes + 8);

    // Bit field data
    // Sub field 1 (8 bits, unsigned)
    uint8_t subField1 = 1; // Example value
    buffer.push_back(subField1); // 1

    // Sub field 2 (8 bits, unsigned)
    uint8_t subField2 = 0; // Example value
    buffer.push_back(subField2); // 0


    // Sub field 3 (16 bits, unsigned)
    uint16_t subField3 = 255; // Example value
    buffer.push_back(static_cast<uint8_t>(subField3 & 0xFF));        // Byte 0
    buffer.push_back(static_cast<uint8_t>((subField3 >> 8) & 0xFF)); // Byte 1

    return buffer;
}

// Create a buffer with bit_field data (with int and char_array fields)
std::vector<uint8_t> createBitFieldBuffer()
{
    std::vector<uint8_t> buffer;

    // Bit field example with 64 bits
    // int_field (32 bits, signed int, big-endian)
    int32_t intField = -123;
    uint8_t intFieldBytes[4];
    std::memcpy(intFieldBytes, &intField, sizeof(int32_t));
    buffer.push_back(intFieldBytes[3]);  // Most significant byte
    buffer.push_back(intFieldBytes[2]);
    buffer.push_back(intFieldBytes[1]);
    buffer.push_back(intFieldBytes[0]);  // Least significant byte

    // char_array_field (32 bits, char array, big-endian)
    std::string charArrayField = "Test";
    for (char c : charArrayField) {
        buffer.push_back(static_cast<uint8_t>(c));
    }
    // Pad remaining bytes to make 32 bits (4 bytes total)
    while (buffer.size() < 8) {
        buffer.push_back(0);
    }

    return buffer;
}

// Helper function to create a test JSON file including a boolean field
void createJsonWithBool(const std::string &filename)
{
    std::ofstream jsonFile(filename);
    jsonFile << R"(
    {
        "endianness": "big",
        "fields": [
            {
                "name": "bool_field",
                "type": "boolean",
                "size": 8
            }
        ]
    }
    )";
    jsonFile.close();
}

// Create a buffer that includes a boolean field
std::vector<uint8_t> createBufferWithBool()
{
    std::vector<uint8_t> buffer;

    // Add boolean field (8 bits, true)
    bool boolField = true;
    buffer.push_back(static_cast<uint8_t>(boolField ? 1 : 0));

    return buffer;
}

// Test with boolean field
TEST(PacketParserTest, BooleanField)
{
    std::vector<uint8_t> buffer = createBufferWithBool();
    std::string jsonFilePath = generateUniqueJsonFileName();
    createJsonWithBool(jsonFilePath);

    PacketParserTest parser(jsonFilePath);
    parser.setBuffer(buffer.data());

    EXPECT_EQ(std::get<bool>(parser.getFieldValue("bool_field")), true);

    // Clean up
    cleanupJsonFile(jsonFilePath);
}

// Test with little endian buffer
TEST(PacketParserTest, LittleEndianBuffer)
{
    std::vector<uint8_t> buffer = createLittleEndianBuffer();
    std::string jsonFilePath = generateUniqueJsonFileName();
    createLittleEndianJson(jsonFilePath);

    PacketParserTest parser(jsonFilePath);
    parser.setBuffer(buffer.data());

    EXPECT_EQ(std::get<uint32_t>(parser.getFieldValue("status")), 10);
    EXPECT_EQ(std::get<std::string>(parser.getFieldValue("message")), 
              "Hello!");
    EXPECT_FLOAT_EQ(std::get<float>(parser.getFieldValue("temperature")), 
                    25.0f);
    EXPECT_FLOAT_EQ(std::get<float>(parser.getFieldValue("pressure")), 
                    50.0f);
    EXPECT_EQ(std::get<int32_t>(parser.getFieldValue("flags")), 15);
    EXPECT_DOUBLE_EQ(std::get<double>(parser.getFieldValue("double_value")),
                     54321.9876);

    // Clean up
    cleanupJsonFile(jsonFilePath);
}

// Test with bit field buffer having int and char array fields
TEST(PacketParserTest, BitFieldBuffer)
{
    std::vector<uint8_t> buffer = createBitFieldBuffer();
    std::string jsonFilePath = generateUniqueJsonFileName();
    createBitFieldJson(jsonFilePath);

    PacketParserTest parser(jsonFilePath);
    parser.setBuffer(buffer.data());

    EXPECT_EQ(std::get<int32_t>(parser.getFieldValue("int_field")), -123);
    EXPECT_EQ(std::get<std::string>(parser.getFieldValue("char_array_field")),
              "Test");
              
    // Clean up
    cleanupJsonFile(jsonFilePath);
}

// Test with empty buffer
TEST(PacketParserTest, EmptyBuffer)
{
    std::vector<uint8_t> emptyBuffer;  // Empty buffer
    std::string jsonFilePath = generateUniqueJsonFileName();
    createTestJson(jsonFilePath);

    // Expect the parser to handle empty buffer gracefully
    EXPECT_THROW(
        {
            try {
                PacketParserTest parser(jsonFilePath);
                parser.setBuffer(emptyBuffer.data());
                parser.getFieldValue("some_field"); 
            }
            catch (const std::runtime_error &e) {
                EXPECT_STREQ(e.what(), "Buffer is null");
                throw;  // Re-throw to let the test framework handle the failure
            }
        },
        std::runtime_error);

    // Clean up
    cleanupJsonFile(jsonFilePath);
}

// Test initializing the parser and using it on two different buffers by setting setBuffer twice
TEST(PacketParserTest, SetBufferTwice)
{
    // First buffer: Normal test buffer without extra data or padding
    std::vector<uint8_t> buffer1 = createTestBuffer();
    std::string jsonFilePath = generateUniqueJsonFileName();
    createTestJson(jsonFilePath);

    PacketParserTest parser(jsonFilePath);
    parser.setBuffer(buffer1.data());

    // Validate fields from the first buffer
    EXPECT_EQ(std::get<uint32_t>(parser.getFieldValue("status")), 5);
    EXPECT_EQ(std::get<std::string>(parser.getFieldValue("message")), 
              "Hello!");
    EXPECT_FLOAT_EQ(std::get<float>(parser.getFieldValue("temperature")), 
                    36.5f);
    EXPECT_FLOAT_EQ(std::get<float>(parser.getFieldValue("pressure")), 
                    101.3f);
    EXPECT_EQ(std::get<int32_t>(parser.getFieldValue("flags")), -5);
    EXPECT_DOUBLE_EQ(std::get<double>(parser.getFieldValue("double_value")), 
                     12345.6789);

    // Now set a second buffer: adding extra data or padding to simulate different conditions
    std::vector<uint8_t> buffer2 = createTestBuffer();
    buffer2.push_back(0xFF);  // Extra data
    buffer2.insert(buffer2.end(), 2, 0xAA);  // Padding

    parser.setBuffer(buffer2.data());  // Set the new buffer

    // Validate fields again, ensuring extra data and padding are ignored
    EXPECT_EQ(std::get<uint32_t>(parser.getFieldValue("status")), 5);
    EXPECT_EQ(std::get<std::string>(parser.getFieldValue("message")), 
              "Hello!");
    EXPECT_FLOAT_EQ(std::get<float>(parser.getFieldValue("temperature")), 
                    36.5f);
    EXPECT_FLOAT_EQ(std::get<float>(parser.getFieldValue("pressure")), 
                    101.3f);
    EXPECT_EQ(std::get<int32_t>(parser.getFieldValue("flags")), -5);
    EXPECT_DOUBLE_EQ(std::get<double>(parser.getFieldValue("double_value")), 
                     12345.6789);

    // Clean up
    cleanupJsonFile(jsonFilePath);
}

// Test that parses the buffer correctly
TEST(PacketParserTest, ParseBuffer)
{
    std::vector<uint8_t> buffer = createTestBuffer();
    std::string jsonFilePath = generateUniqueJsonFileName();
    createTestJson(jsonFilePath);

    PacketParserTest parser(jsonFilePath);
    parser.setBuffer(buffer.data());

    EXPECT_EQ(std::get<uint32_t>(parser.getFieldValue("status")), 5);
    EXPECT_EQ(std::get<std::string>(parser.getFieldValue("message")), 
              "Hello!");
    EXPECT_FLOAT_EQ(std::get<float>(parser.getFieldValue("temperature")), 
                    36.5f);
    EXPECT_FLOAT_EQ(std::get<float>(parser.getFieldValue("pressure")), 
                    101.3f);
    EXPECT_EQ(std::get<int32_t>(parser.getFieldValue("flags")), -5);
    EXPECT_DOUBLE_EQ(std::get<double>(parser.getFieldValue("double_value")), 
                     12345.6789);

    // Clean up
    cleanupJsonFile(jsonFilePath);
}

// Test that field type detection works
TEST(PacketParserTest, FieldTypeDetection)
{
    std::string jsonFilePath = generateUniqueJsonFileName();
    createTestJson(jsonFilePath);

    PacketParserTest parser(jsonFilePath);

    EXPECT_EQ(parser.getFieldType("unsigned_int"), FieldType::UNSIGNED_INT);
    EXPECT_EQ(parser.getFieldType("char_array"), FieldType::CHAR_ARRAY);
    EXPECT_EQ(parser.getFieldType("float_fixed"), FieldType::FLOAT_FIXED);
    EXPECT_EQ(parser.getFieldType("float_mantissa"), FieldType::FLOAT_MANTISSA);
    EXPECT_EQ(parser.getFieldType("bit_field"), FieldType::BIT_FIELD);
    EXPECT_EQ(parser.getFieldType("double"), FieldType::DOUBLE);
    EXPECT_EQ(parser.getFieldType("signed_int"), FieldType::SIGNED_INT);
    EXPECT_EQ(parser.getFieldType("unknown"), FieldType::UNKNOWN);

    // Clean up
    cleanupJsonFile(jsonFilePath);
}

// Test that invalid field names throw an error
TEST(PacketParserTest, InvalidFieldName)
{
    std::vector<uint8_t> buffer = createTestBuffer();
    std::string jsonFilePath = generateUniqueJsonFileName();
    createTestJson(jsonFilePath);

    PacketParserTest parser(jsonFilePath);
    parser.setBuffer(buffer.data());

    EXPECT_THROW(parser.getFieldValue("invalid_field"), std::runtime_error);

    // Clean up
    cleanupJsonFile(jsonFilePath);
}

// Test that invalid JSON file throws an error
TEST(PacketParserTest, InvalidJsonFile)
{
    std::vector<uint8_t> buffer = createTestBuffer();
    std::string invalidJsonFilePath = "invalid_test.json";

    EXPECT_THROW(PacketParser parser(invalidJsonFilePath),
                 std::runtime_error);

    // No need to clean up as this file was never created
}

// Test extracting bits from a byte array
TEST(PacketParserTest, ExtractBits)
{
    std::vector<uint8_t> buffer = {0xAB, 0xCD,
                                   0xEF};  // 10101011 11001101 11101111
    std::string jsonFilePath = generateUniqueJsonFileName();
    createExtractBitsJson(jsonFilePath);

    PacketParserTest parser(jsonFilePath);
    parser.setBuffer(buffer.data());

    auto extractedBits =
        parser.extractBits(buffer.data(), 0, 8);  // 8 bits from position 0
    EXPECT_EQ(extractedBits[0], 0xAB);              // Should be 10101011

    extractedBits =
        parser.extractBits(buffer.data(), 8, 8);  // 8 bits from position 8
    EXPECT_EQ(extractedBits[0], 0xCD);              // Should be 11001101

    extractedBits =
        parser.extractBits(buffer.data(), 16, 16);  // 16 bits from position 16
    EXPECT_EQ(extractedBits[0], 0xEF);                // Should be 11101111

    cleanupJsonFile({jsonFilePath});
}

// Test parsing a buffer with correct data
TEST(PacketParserTest, TestBufferCommunication)
{
    std::vector<uint8_t> buffer = createTestCommunicationBuffer();
    std::string jsonFilePath = generateUniqueJsonFileName();
    createTestCommunicationJson(jsonFilePath);

    try {
        PacketParserTest parser(jsonFilePath);
        parser.setBuffer(buffer.data());

        // Assert values from the parsed buffer
        EXPECT_EQ(std::get<uint32_t>(parser.getFieldValue("MessageType")), 0);
        EXPECT_EQ(std::get<uint32_t>(parser.getFieldValue("Level")), 4);
        EXPECT_EQ(std::get<uint32_t>(parser.getFieldValue("ObjectType")), 6);
        EXPECT_FLOAT_EQ(std::get<float>(parser.getFieldValue("ObjectDistance")),
                        123.456f);
        EXPECT_EQ(std::get<uint32_t>(parser.getFieldValue("CarSpeed")), 60);
        EXPECT_EQ(std::get<uint32_t>(parser.getFieldValue("ObjectSpeed")), 80);
    }
    catch (const std::exception &e) {
        FAIL() << "Exception occurred: " << e.what();
    }

    // Clean up
    cleanupJsonFile(jsonFilePath);
}

// Test parsing a buffer with padding
TEST(PacketParserTest, TestBufferCommunicationWithPadding)
{
    std::vector<uint8_t> buffer = createTestCommunicationBuffer();
    buffer.insert(buffer.end(), 2, 0xAA);  // Add padding

    std::string jsonFilePath = generateUniqueJsonFileName();
    createTestCommunicationJson(jsonFilePath);

    try {
        PacketParserTest parser(jsonFilePath);
        parser.setBuffer(buffer.data());

        // Assert values from the parsed buffer ignoring the padding
        EXPECT_EQ(std::get<uint32_t>(parser.getFieldValue("MessageType")), 0);
        EXPECT_EQ(std::get<uint32_t>(parser.getFieldValue("Level")), 4);
        EXPECT_EQ(std::get<uint32_t>(parser.getFieldValue("ObjectType")), 6);
        EXPECT_FLOAT_EQ(std::get<float>(parser.getFieldValue("ObjectDistance")),
                        123.456f);
        EXPECT_EQ(std::get<uint32_t>(parser.getFieldValue("CarSpeed")), 60);
        EXPECT_EQ(std::get<uint32_t>(parser.getFieldValue("ObjectSpeed")), 80);
    }
    catch (const std::exception &e) {
        FAIL() << "Exception occurred: " << e.what();
    }

    // Clean up
    cleanupJsonFile(jsonFilePath);
}

// Test that empty buffer initialization throws an error
TEST(PacketParserTest, EmptyBufferInitialization)
{
    std::string jsonFilePath = generateUniqueJsonFileName();
    
    createTestJson(jsonFilePath);

    PacketParserTest parser(jsonFilePath);

    EXPECT_THROW({
        try {
            parser.getFieldValue("some_field"); // Attempt to access a field
        } catch (const std::runtime_error &e) {
            EXPECT_STREQ(e.what(), "Buffer is null");
            throw;  // Re-throw to let the test framework handle the failure
        }
    }, std::runtime_error);

    // Clean up the generated JSON file
    cleanupJsonFile({jsonFilePath});
}

// Test for getAllFieldValues
TEST(PacketParserTest, GetAllFieldValues) {
    // Create a sample buffer and JSON file
    std::vector<uint8_t> buffer = createLittleEndianTestBuffer(); // Define your buffer creation logic
    std::string jsonFilePath = generateUniqueJsonFileName();
    createLittleEndianBitFieldJson(jsonFilePath); // Create the JSON file with field definitions

    // Initialize the PacketParser with the JSON file
    PacketParserTest parser(jsonFilePath);
    parser.setBuffer(buffer.data());

    // Get all field values
    auto allValues = parser.getAllFieldValues();

    // Validate extracted regular field values
    EXPECT_EQ(std::get<unsigned int>(allValues["status"]), 10);
    EXPECT_EQ(std::get<std::string>(allValues["message"]), "Hello!");
    EXPECT_FLOAT_EQ(std::get<float>(allValues["temperature"]), 25.0f);
    EXPECT_FLOAT_EQ(std::get<float>(allValues["pressure"]), 50.0f);
    EXPECT_EQ(std::get<int>(allValues["flags"]), 15);
    EXPECT_DOUBLE_EQ(std::get<double>(allValues["double_value"]), 54321.9876);

    // Validate BitField values (ensure they're included in the flat map)
    EXPECT_TRUE(allValues.count("sub_field_1") > 0) << "Key 'sub_field_1' not found.";
    EXPECT_EQ(std::get<unsigned int>(allValues["sub_field_1"]), 1);
    
    EXPECT_TRUE(allValues.count("sub_field_2") > 0) << "Key 'sub_field_2' not found.";
    EXPECT_EQ(std::get<unsigned int>(allValues["sub_field_2"]), 0);
    
    EXPECT_TRUE(allValues.count("sub_field_3") > 0) << "Key 'sub_field_3' not found.";
    EXPECT_EQ(std::get<unsigned int>(allValues["sub_field_3"]), 255);

    // Clean up
    cleanupJsonFile(jsonFilePath);
}

// Test for printFieldValues with BitField
TEST(PacketParserTest, PrintFieldValuesWithBitField)
{
    std::vector<uint8_t> buffer = createLittleEndianTestBuffer(); // Assuming this function is already defined
    std::string jsonFilePath = generateUniqueJsonFileName();
    createLittleEndianBitFieldJson(jsonFilePath); // Use the new function

    PacketParserTest parser(jsonFilePath);
    parser.setBuffer(buffer.data());

    // Get all field values
    auto allValues = parser.getAllFieldValues();

    // Redirect output to a string stream
    std::ostringstream outputStream;
    std::streambuf *originalCoutBuffer = std::cout.rdbuf(outputStream.rdbuf());

    // Print field values
    parser.printFieldValues(allValues);

    // Restore original cout buffer
    std::cout.rdbuf(originalCoutBuffer);

    // Get the printed output
    std::string output = outputStream.str();

    // Print the output for debugging
    std::cout << "Captured Output: \n" << output << std::endl;

    // Validate the printed output - adjust to match the format from printFieldValues
    EXPECT_NE(output.find("status : 10"), std::string::npos);
    EXPECT_NE(output.find("message : Hello!"), std::string::npos);
    EXPECT_NE(output.find("temperature : 25.00000000"), std::string::npos);  // Adjust precision
    EXPECT_NE(output.find("pressure : 50.00000000"), std::string::npos);     // Adjust precision
    EXPECT_NE(output.find("flags : 15"), std::string::npos);

    // Check double_value by truncating or using regex to match precision
    EXPECT_NE(output.find("double_value : 54321.9876"), std::string::npos);

    // Validate BitField output - ensure the buffer and parsing are correct
    EXPECT_NE(output.find("sub_field_1 : 1"), std::string::npos);
    EXPECT_NE(output.find("sub_field_2 : 0"), std::string::npos);
    EXPECT_NE(output.find("sub_field_3 : 255"), std::string::npos);

    // Clean up
    cleanupJsonFile(jsonFilePath);
}

// Test for loadJson and default values
TEST(PacketParserTest, LoadJson_And_CheckValues)
{
    std::string jsonFilePath = generateUniqueJsonFileName();
    createJsonWithDefaultValues(jsonFilePath);

    PacketParserTest parser(jsonFilePath);

    // Check if the fields were populated correctly
    ASSERT_EQ(parser.getFields().size(), 6);  // Should match the number of fields in the JSON

    EXPECT_EQ(parser.getFields()[0].name, "unsignedIntField");
    EXPECT_EQ(std::get<uint32_t>(parser.getFields()[0].defaultValue), 100);

    EXPECT_EQ(parser.getFields()[1].name, "intField");
    EXPECT_EQ(std::get<int32_t>(parser.getFields()[1].defaultValue), -50);

    EXPECT_EQ(parser.getFields()[2].name, "stringField");
    EXPECT_EQ(std::get<std::string>(parser.getFields()[2].defaultValue), "");

    EXPECT_EQ(parser.getFields()[3].name, "floatField");
    EXPECT_FLOAT_EQ(std::get<float>(parser.getFields()[3].defaultValue), 3.14f);

    EXPECT_EQ(parser.getFields()[4].name, "doubleField");
    EXPECT_DOUBLE_EQ(std::get<double>(parser.getFields()[4].defaultValue), 2.718);

    EXPECT_EQ(parser.getFields()[5].name, "boolField");
    EXPECT_EQ(std::get<bool>(parser.getFields()[5].defaultValue), true);

    // Test for an unknown field type to throw an error
    EXPECT_THROW({
        FieldValue value = parser.getDefaultValueByType("unknown_type"); 
    }, std::runtime_error);

    // Clean up
    cleanupJsonFile(jsonFilePath);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}