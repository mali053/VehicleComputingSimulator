#include "packet_parser.h"
#include <cstring>
#include <fstream>
#include <gtest/gtest.h>
#include <vector>
#include <chrono>
#include <sstream>
#include <iostream>
#include <bitset>
#include <filesystem>

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

// Helper function to clean up all generated JSON files
void cleanupJsonFiles(const std::vector<std::string> &filenames)
{
    for (const auto &filename : filenames) {
        if (std::filesystem::exists(filename)) {
            std::filesystem::remove(filename);
        }
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
    std::vector<std::string> jsonFiles;
    std::vector<uint8_t> buffer = createBufferWithBool();
    std::string jsonFilePath = generateUniqueJsonFileName();
    createJsonWithBool(jsonFilePath);
    jsonFiles.push_back(jsonFilePath);

    PacketParser parser(jsonFilePath, buffer.data());

    EXPECT_EQ(*static_cast<bool *>(parser.getFieldValue("bool_field")), true);

    // Clean up
    cleanupJsonFiles(jsonFiles);
}

// Test with little endian buffer
TEST(PacketParserTest, LittleEndianBuffer)
{
    std::vector<std::string> jsonFiles;
    std::vector<uint8_t> buffer = createLittleEndianBuffer();
    std::string jsonFilePath = generateUniqueJsonFileName();
    createLittleEndianJson(jsonFilePath);
    jsonFiles.push_back(jsonFilePath);

    PacketParser parser(jsonFilePath, buffer.data());

    EXPECT_EQ(*static_cast<uint32_t *>(parser.getFieldValue("status")), 10);
    EXPECT_EQ(*static_cast<std::string *>(parser.getFieldValue("message")),
              "Hello!");
    EXPECT_FLOAT_EQ(*static_cast<float *>(parser.getFieldValue("temperature")),
                    25.0f);
    EXPECT_FLOAT_EQ(*static_cast<float *>(parser.getFieldValue("pressure")),
                    50.0f);
    EXPECT_EQ(*static_cast<int32_t *>(parser.getFieldValue("flags")), 15);
    EXPECT_DOUBLE_EQ(
        *static_cast<double *>(parser.getFieldValue("double_value")),
        54321.9876);

    // Clean up
    cleanupJsonFiles(jsonFiles);
}

// Test with bit field buffer having int and char array fields
TEST(PacketParserTest, BitFieldBuffer)
{
    std::vector<std::string> jsonFiles;
    std::vector<uint8_t> buffer = createBitFieldBuffer();
    std::string jsonFilePath = generateUniqueJsonFileName();
    createBitFieldJson(jsonFilePath);
    jsonFiles.push_back(jsonFilePath);

    PacketParser parser(jsonFilePath, buffer.data());

    EXPECT_EQ(*static_cast<int32_t *>(parser.getFieldValue("int_field")), -123);
    EXPECT_EQ(
        *static_cast<std::string *>(parser.getFieldValue("char_array_field")),
        "Test");

    // Clean up
    cleanupJsonFiles(jsonFiles);
}

// Test with empty buffer
TEST(PacketParserTest, EmptyBuffer)
{
    std::vector<std::string> jsonFiles;
    std::vector<uint8_t> emptyBuffer;  // Empty buffer
    std::string jsonFilePath = generateUniqueJsonFileName();
    createTestJson(jsonFilePath);
    jsonFiles.push_back(jsonFilePath);

    // Expect the parser to handle empty buffer gracefully
    EXPECT_THROW(
        {
            try {
                PacketParser parser(jsonFilePath, emptyBuffer.data());
            }
            catch (const std::runtime_error &e) {
                EXPECT_STREQ(e.what(), "Buffer is null");
                throw;  // Re-throw to let the test framework handle the failure
            }
        },
        std::runtime_error);

    // Clean up
    cleanupJsonFiles(jsonFiles);
}

// Test with extra data in buffer
TEST(PacketParserTest, ExtraDataInBuffer)
{
    std::vector<std::string> jsonFiles;
    std::vector<uint8_t> buffer = createTestBuffer();
    buffer.push_back(0xFF);

    std::string jsonFilePath = generateUniqueJsonFileName();
    createTestJson(jsonFilePath);
    jsonFiles.push_back(jsonFilePath);

    PacketParser parser(jsonFilePath, buffer.data());

    // Validate fields ignoring the extra data
    EXPECT_EQ(*static_cast<uint32_t *>(parser.getFieldValue("status")), 5);
    EXPECT_EQ(*static_cast<std::string *>(parser.getFieldValue("message")),
              "Hello!");
    EXPECT_FLOAT_EQ(*static_cast<float *>(parser.getFieldValue("temperature")),
                    36.5f);
    EXPECT_FLOAT_EQ(*static_cast<float *>(parser.getFieldValue("pressure")),
                    101.3f);
    EXPECT_EQ(*static_cast<int32_t *>(parser.getFieldValue("flags")), -5);
    EXPECT_DOUBLE_EQ(
        *static_cast<double *>(parser.getFieldValue("double_value")),
        12345.6789);

    // Clean up
    cleanupJsonFiles(jsonFiles);
}

// Test with buffer containing padding
TEST(PacketParserTest, BufferWithPadding)
{
    std::vector<std::string> jsonFiles;
    std::vector<uint8_t> buffer = createTestBuffer();
    buffer.insert(buffer.end(), 2, 0xAA);

    std::string jsonFilePath = generateUniqueJsonFileName();
    createTestJson(jsonFilePath);
    jsonFiles.push_back(jsonFilePath);

    PacketParser parser(jsonFilePath, buffer.data());

    // Validate fields ignoring the padding
    EXPECT_EQ(*static_cast<uint32_t *>(parser.getFieldValue("status")), 5);
    EXPECT_EQ(*static_cast<std::string *>(parser.getFieldValue("message")),
              "Hello!");
    EXPECT_FLOAT_EQ(*static_cast<float *>(parser.getFieldValue("temperature")),
                    36.5f);
    EXPECT_FLOAT_EQ(*static_cast<float *>(parser.getFieldValue("pressure")),
                    101.3f);
    EXPECT_EQ(*static_cast<int32_t *>(parser.getFieldValue("flags")), -5);
    EXPECT_DOUBLE_EQ(
        *static_cast<double *>(parser.getFieldValue("double_value")),
        12345.6789);

    // Clean up
    cleanupJsonFiles(jsonFiles);
}

// Test that parses the buffer correctly
TEST(PacketParserTest, ParseBuffer)
{
    std::vector<std::string> jsonFiles;
    std::vector<uint8_t> buffer = createTestBuffer();
    std::string jsonFilePath = generateUniqueJsonFileName();
    createTestJson(jsonFilePath);
    jsonFiles.push_back(jsonFilePath);

    PacketParser parser(jsonFilePath, buffer.data());

    EXPECT_EQ(*static_cast<uint32_t *>(parser.getFieldValue("status")), 5);
    EXPECT_EQ(*static_cast<std::string *>(parser.getFieldValue("message")),
              "Hello!");
    EXPECT_FLOAT_EQ(*static_cast<float *>(parser.getFieldValue("temperature")),
                    36.5f);
    EXPECT_FLOAT_EQ(*static_cast<float *>(parser.getFieldValue("pressure")),
                    101.3f);
    EXPECT_EQ(*static_cast<int32_t *>(parser.getFieldValue("flags")), -5);
    EXPECT_DOUBLE_EQ(
        *static_cast<double *>(parser.getFieldValue("double_value")),
        12345.6789);

    // Clean up
    cleanupJsonFiles(jsonFiles);
}

// Test that field type detection works
TEST(PacketParserTest, FieldTypeDetection)
{
    std::vector<std::string> jsonFiles;
    std::string jsonFilePath = generateUniqueJsonFileName();
    createTestJson(jsonFilePath);
    jsonFiles.push_back(jsonFilePath);

    // Use a non-null buffer to avoid issues with the parser expecting one
    std::vector<uint8_t> dummyBuffer(1, 0);  // Small dummy buffer
    EXPECT_NO_THROW({ PacketParser parser(jsonFilePath, dummyBuffer.data()); });

    PacketParser parser(jsonFilePath, dummyBuffer.data());

    EXPECT_EQ(parser.getFieldType("unsigned_int"), FieldType::UNSIGNED_INT);
    EXPECT_EQ(parser.getFieldType("char_array"), FieldType::CHAR_ARRAY);
    EXPECT_EQ(parser.getFieldType("float_fixed"), FieldType::FLOAT_FIXED);
    EXPECT_EQ(parser.getFieldType("float_mantissa"), FieldType::FLOAT_MANTISSA);
    EXPECT_EQ(parser.getFieldType("bit_field"), FieldType::BIT_FIELD);
    EXPECT_EQ(parser.getFieldType("double"), FieldType::DOUBLE);
    EXPECT_EQ(parser.getFieldType("signed_int"), FieldType::SIGNED_INT);
    EXPECT_EQ(parser.getFieldType("unknown"), FieldType::UNKNOWN);

    // Clean up
    cleanupJsonFiles(jsonFiles);
}

// Test that invalid field names throw an error
TEST(PacketParserTest, InvalidFieldName)
{
    std::vector<std::string> jsonFiles;
    std::vector<uint8_t> buffer = createTestBuffer();
    std::string jsonFilePath = generateUniqueJsonFileName();
    createTestJson(jsonFilePath);
    jsonFiles.push_back(jsonFilePath);

    PacketParser parser(jsonFilePath, buffer.data());

    EXPECT_THROW(parser.getFieldValue("invalid_field"), std::runtime_error);

    // Clean up
    cleanupJsonFiles(jsonFiles);
}

// Test that invalid JSON file throws an error
TEST(PacketParserTest, InvalidJsonFile)
{
    std::vector<uint8_t> buffer = createTestBuffer();
    std::string invalidJsonFilePath = "invalid_test.json";

    EXPECT_THROW(PacketParser parser(invalidJsonFilePath, buffer.data()),
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

    PacketParser parser(jsonFilePath, buffer.data());

    uint8_t *extractedBits =
        parser.extractBits(buffer.data(), 0, 8);  // 8 bits from position 0
    EXPECT_EQ(*extractedBits, 0xAB);              // Should be 10101011
    std::free(extractedBits);

    extractedBits =
        parser.extractBits(buffer.data(), 8, 8);  // 8 bits from position 8
    EXPECT_EQ(*extractedBits, 0xCD);              // Should be 11001101
    std::free(extractedBits);

    extractedBits =
        parser.extractBits(buffer.data(), 16, 16);  // 16 bits from position 16
    EXPECT_EQ(*extractedBits, 0xEF);                // Should be 11101111
    std::free(extractedBits);

    // ניקוי JSON שנוצר
    cleanupJsonFiles({jsonFilePath});
}

// Test parsing a buffer with correct data
TEST(PacketParserTest, TestBufferCommunication)
{
    std::vector<std::string> jsonFiles;
    std::vector<uint8_t> buffer = createTestCommunicationBuffer();
    std::string jsonFilePath = generateUniqueJsonFileName();
    createTestCommunicationJson(jsonFilePath);
    jsonFiles.push_back(jsonFilePath);

    try {
        PacketParser parser(jsonFilePath, buffer.data());

        // Assert values from the parsed buffer
        EXPECT_EQ(*static_cast<uint32_t *>(parser.getFieldValue("MessageType")),
                  0);
        EXPECT_EQ(*static_cast<uint32_t *>(parser.getFieldValue("Level")), 4);
        EXPECT_EQ(*static_cast<uint32_t *>(parser.getFieldValue("ObjectType")),
                  6);
        EXPECT_FLOAT_EQ(
            *static_cast<float *>(parser.getFieldValue("ObjectDistance")),
            123.456f);
        EXPECT_EQ(*static_cast<uint32_t *>(parser.getFieldValue("CarSpeed")),
                  60);
        EXPECT_EQ(*static_cast<uint32_t *>(parser.getFieldValue("ObjectSpeed")),
                  80);
    }
    catch (const std::exception &e) {
        FAIL() << "Exception occurred: " << e.what();
    }

    // Clean up
    cleanupJsonFiles(jsonFiles);
}

// Test parsing a buffer with padding
TEST(PacketParserTest, TestBufferCommunicationWithPadding)
{
    std::vector<std::string> jsonFiles;
    std::vector<uint8_t> buffer = createTestCommunicationBuffer();
    buffer.insert(buffer.end(), 2, 0xAA);  // Add padding

    std::string jsonFilePath = generateUniqueJsonFileName();
    createTestCommunicationJson(jsonFilePath);
    jsonFiles.push_back(jsonFilePath);

    try {
        PacketParser parser(jsonFilePath, buffer.data());

        // Assert values from the parsed buffer ignoring the padding
        EXPECT_EQ(*static_cast<uint32_t *>(parser.getFieldValue("MessageType")),
                  0);
        EXPECT_EQ(*static_cast<uint32_t *>(parser.getFieldValue("Level")), 4);
        EXPECT_EQ(*static_cast<uint32_t *>(parser.getFieldValue("ObjectType")),
                  6);
        EXPECT_FLOAT_EQ(
            *static_cast<float *>(parser.getFieldValue("ObjectDistance")),
            123.456f);
        EXPECT_EQ(*static_cast<uint32_t *>(parser.getFieldValue("CarSpeed")),
                  60);
        EXPECT_EQ(*static_cast<uint32_t *>(parser.getFieldValue("ObjectSpeed")),
                  80);
    }
    catch (const std::exception &e) {
        FAIL() << "Exception occurred: " << e.what();
    }

    // Clean up
    cleanupJsonFiles(jsonFiles);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}