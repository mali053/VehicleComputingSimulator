#include "packet_parser.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stdexcept>

using json = nlohmann::json;

PacketParser::~PacketParser()
{
    // Clean up buffer if necessary
}

FieldType PacketParser::getFieldType(const std::string &typeStr) const
{
    if (typeStr == "unsigned_int")
        return FieldType::UNSIGNED_INT;
    if (typeStr == "char_array")
        return FieldType::CHAR_ARRAY;
    if (typeStr == "float_fixed")
        return FieldType::FLOAT_FIXED;
    if (typeStr == "float_mantissa")
        return FieldType::FLOAT_MANTISSA;
    if (typeStr == "bit_field")
        return FieldType::BIT_FIELD;
    if (typeStr == "double")
        return FieldType::DOUBLE;
    if (typeStr == "signed_int")
        return FieldType::SIGNED_INT;
    if (typeStr == "boolean")
        return FieldType::BOOLEAN;

    return FieldType::UNKNOWN;
}

PacketParser::PacketParser(const std::string &jsonFilePath, const void *buffer)
    : _buffer(buffer)
{
    loadJson(jsonFilePath);
    if (buffer == nullptr) {
        throw std::runtime_error("Buffer is null");
    }
}

void PacketParser::validateFieldSize(const std::string &type,
                                     size_t bitLength) const
{
    if ((type == "float_fixed" || type == "float_mantissa") &&
        bitLength != 32) {
        throw std::runtime_error("Float types must be 32 bits.");
    }
    if (type == "signed_int" && bitLength != 32) {
        throw std::runtime_error("Signed integers must be 32 bits.");
    }
    if (type == "boolean" && bitLength != 8) {
        throw std::runtime_error("Boolean types must be 8 bits.");
    }
    if (type == "double" && bitLength != 64) {
        throw std::runtime_error("Double types must be 64 bits.");
    }
    // Add more validations if needed
}

void PacketParser::loadJson(const std::string &jsonFilePath)
{
    std::ifstream jsonFile(jsonFilePath);
    if (!jsonFile.is_open()) {
        throw std::runtime_error("Unable to open JSON file");
    }

    json jsonData;
    jsonFile >> jsonData;

    endianness = jsonData["endianness"];

    size_t currentOffset = 0;

    for (const auto &fieldJson : jsonData["fields"]) {
        size_t fieldSize = fieldJson["size"];
        std::string fieldType = fieldJson["type"];

        validateFieldSize(fieldType, fieldSize);

        size_t byteLength = (fieldSize + 7) / 8;  // Convert bits to bytes
        Field field;
        field.name = fieldJson["name"];
        field.type = fieldType;
        field.size = fieldSize;
        field.offset = currentOffset;

        fields.push_back(field);

        if (fieldType == "bit_field") {
            BitField bitField;
            bitField.name = fieldJson["name"];
            bitField.size = fieldJson["size"];
            bitField.offset = currentOffset;

            size_t bitFieldOffset = 0;

            for (const auto &subFieldJson : fieldJson["fields"]) {
                std::string subFieldType = subFieldJson["type"];
                size_t subFieldSize = subFieldJson["size"];

                validateFieldSize(subFieldType, subFieldSize);

                Field subField;
                subField.name = subFieldJson["name"];
                subField.type = subFieldType;
                subField.size = subFieldSize;
                subField.offset = bitFieldOffset;

                bitField.fields.push_back(subField);
                bitFieldOffset += subField.size;
            }

            bitFields.push_back(bitField);
        }

        currentOffset += byteLength;
    }
}

uint32_t PacketParser::decodeUnsignedInt(const uint8_t *data,
                                         size_t bitLength) const
{
    size_t byteLength = (bitLength + 7) / 8;  // Convert bits to bytes
    uint32_t value = 0;

    for (size_t i = 0; i < byteLength; i++) {
        value |= static_cast<uint32_t>(data[i]) << (i * 8);
    }

    if (endianness == "big") {
        value = __builtin_bswap32(value);
    }

    uint32_t mask = bitLength == 32 ? 0xFFFFFFFF : ((1U << bitLength) - 1);

    return value & mask;
}

int32_t PacketParser::decodeSignedInt(const uint8_t *data,
                                      size_t bitLength) const
{
    size_t byteLength = (bitLength + 7) / 8;  // Convert bits to bytes
    int32_t value = 0;

    // Read the value from the data array
    for (size_t i = 0; i < byteLength; i++) {
        value |= static_cast<int32_t>(data[i]) << (i * 8);
    }

    // Handle endianess
    if (endianness == "big") {
        value = __builtin_bswap32(value);
    }

    if (bitLength < 32) {
        // Sign extend the value
        int32_t mask = (1 << (bitLength - 1));  // Mask to get the sign bit
        if (value & mask) {                     // If the sign bit is set
            value |= ~((1 << bitLength) - 1);   // Extend the sign bit
        }
    }

    return value;
}

template <typename T>
T decodeValue(const uint8_t *data, size_t bitLength,
              const std::string &endianness)
{
    size_t byteLength = (bitLength + 7) / 8;  // Convert bits to bytes
    uint64_t tempValue = 0;

    // Read bytes into tempValue
    for (size_t i = 0; i < byteLength; i++) {
        tempValue |= static_cast<uint64_t>(data[i]) << (i * 8);
    }

    // Handle endianess
    if (endianness == "big") {
        tempValue = (bitLength == 32) ? __builtin_bswap32(tempValue)
                                      : __builtin_bswap64(tempValue);
    }

    // Mask the value if bitLength < 32 or < 64
    if (bitLength < 64) {
        tempValue &= (1ULL << bitLength) - 1;
    }

    // Convert to the appropriate type
    T value;
    std::memcpy(&value, &tempValue, sizeof(value));
    return value;
}

float PacketParser::decodeFloat(const uint8_t *data, size_t bitLength) const
{
    return decodeValue<float>(data, bitLength, endianness);
}

std::string PacketParser::decodeCharArray(const uint8_t *data,
                                          size_t byteLength) const
{
    std::string msg(reinterpret_cast<const char *>(data), byteLength);
    msg.erase(std::find(msg.begin(), msg.end(), '\0'), msg.end());
    return msg;
}

double PacketParser::decodeDouble(const uint8_t *data, size_t bitLength) const
{
    return decodeValue<double>(data, bitLength, endianness);
}

bool PacketParser::decodeBoolean(const uint8_t *data, size_t bitLength) const
{
    if (bitLength == 0 || bitLength > 8) {
        throw std::runtime_error(
            "Invalid bit length. It should be between 1 and 8 bits.");
    }

    uint8_t byte = data[0];

    // Convert bitLength to zero-based index from the left
    size_t index = 8 - bitLength;

    // Extract the bit value and convert to bool
    bool bitValue = (byte >> index) & 1;

    return bitValue;
}

void *PacketParser::getFieldValue(const std::string &fieldName) const
{
    auto it = std::find_if(
        fields.begin(), fields.end(),
        [&fieldName](const Field &field) { return field.name == fieldName; });

    if (it != fields.end() && it->type != "bit_field") {
        const Field &field = *it;
        const uint8_t *data =
            static_cast<const uint8_t *>(_buffer) + field.offset;
        switch (getFieldType(field.type)) {
            case FieldType::UNSIGNED_INT: {
                uint32_t *value =
                    new uint32_t(decodeUnsignedInt(data, field.size));
                return value;
            }
            case FieldType::SIGNED_INT: {
                int32_t *value = new int32_t(decodeSignedInt(data, field.size));
                return value;
            }
            case FieldType::CHAR_ARRAY: {
                std::string *value = new std::string(
                    decodeCharArray(data, (field.size + 7) / 8));
                return value;
            }
            case FieldType::FLOAT_FIXED:
            case FieldType::FLOAT_MANTISSA: {
                float *value = new float(decodeFloat(data, field.size));
                return value;
            }
            case FieldType::DOUBLE: {
                double *value = new double(decodeDouble(data, field.size));
                return value;
            }
            case FieldType::BOOLEAN: {
                bool *value = new bool(decodeBoolean(data, field.size));
                return value;
            }
            case FieldType::UNKNOWN:
            default:
                throw std::runtime_error("Unknown field type");
        }
    }
    else {
        for (const auto &bitField : bitFields) {
            const uint8_t *bitFieldData =
                static_cast<const uint8_t *>(_buffer) + bitField.offset;

            auto subFieldIt =
                std::find_if(bitField.fields.begin(), bitField.fields.end(),
                             [&fieldName](const Field &subField) {
                                 return subField.name == fieldName;
                             });

            if (subFieldIt != bitField.fields.end()) {
                const Field &subField = *subFieldIt;
                uint8_t *extractedBits =
                    extractBits(bitFieldData, subField.offset, subField.size);
                switch (getFieldType(subField.type)) {
                    case FieldType::UNSIGNED_INT: {
                        uint32_t value =
                            decodeUnsignedInt(extractedBits, subField.size);
                        return new uint32_t(value);
                    }
                    case FieldType::SIGNED_INT: {
                        uint32_t value =
                            decodeSignedInt(extractedBits, subField.size);
                        return new uint32_t(value);
                    }
                    case FieldType::CHAR_ARRAY: {
                        std::string value = decodeCharArray(
                            extractedBits, (subField.size + 7) / 8);
                        return new std::string(value);
                    }
                    case FieldType::FLOAT_FIXED:
                    case FieldType::FLOAT_MANTISSA: {
                        float value = decodeFloat(extractedBits, subField.size);
                        return new float(value);
                    }
                    case FieldType::DOUBLE: {
                        double value =
                            decodeDouble(extractedBits, subField.size);
                        return new double(value);
                    }
                    case FieldType::BOOLEAN: {
                        bool value =
                            decodeBoolean(extractedBits, subField.size);
                        return new bool(value);
                    }
                    case FieldType::UNKNOWN:
                    default:
                        throw std::runtime_error(
                            "Unknown field type in bit field");
                }
            }
        }

        throw std::runtime_error("Field not found in any bit field");
    }
    return NULL;
}

std::map<std::string, FieldValue> PacketParser::getAllFieldValues() const
{
    std::map<std::string, FieldValue> allValues;

    for (const auto &field : fields) {
        try {
            FieldType type = getFieldType(field.type);
            switch (type) {
                case FieldType::SIGNED_INT: {
                    void *valuePtr = getFieldValue(field.name);
                    allValues[field.name] = *static_cast<int32_t *>(valuePtr);
                    delete static_cast<int32_t *>(valuePtr);
                    break;
                }
                case FieldType::UNSIGNED_INT: {
                    void *valuePtr = getFieldValue(field.name);
                    allValues[field.name] = *static_cast<uint32_t *>(valuePtr);
                    delete static_cast<uint32_t *>(valuePtr);
                    break;
                }
                case FieldType::CHAR_ARRAY: {
                    void *valuePtr = getFieldValue(field.name);
                    allValues[field.name] =
                        *static_cast<std::string *>(valuePtr);
                    delete static_cast<std::string *>(valuePtr);
                    break;
                }
                case FieldType::FLOAT_FIXED:
                case FieldType::FLOAT_MANTISSA: {
                    void *valuePtr = getFieldValue(field.name);
                    allValues[field.name] = *static_cast<float *>(valuePtr);
                    delete static_cast<float *>(valuePtr);
                    break;
                }
                case FieldType::DOUBLE: {
                    void *valuePtr = getFieldValue(field.name);
                    allValues[field.name] = *static_cast<double *>(valuePtr);
                    delete static_cast<double *>(valuePtr);
                    break;
                }
                case FieldType::BOOLEAN: {
                    void *valuePtr = getFieldValue(field.name);
                    allValues[field.name] = *static_cast<bool *>(valuePtr);
                    delete static_cast<bool *>(valuePtr);
                    break;
                }
                case FieldType::BIT_FIELD: {
                    std::map<std::string, FieldValue> bitFieldValues;

                    auto bitFieldIt =
                        std::find_if(bitFields.begin(), bitFields.end(),
                                     [&field](const BitField &bitField) {
                                         return bitField.name == field.name;
                                     });

                    if (bitFieldIt == bitFields.end()) {
                        throw std::runtime_error("Field not found");
                    }

                    const BitField &bitField = *bitFieldIt;

                    for (const auto &subField : bitField.fields) {
                        void *subFieldValuePtr = getFieldValue(subField.name);

                        switch (getFieldType(subField.type)) {
                            case FieldType::SIGNED_INT: {
                                allValues[subField.name] =
                                    *static_cast<int32_t *>(subFieldValuePtr);
                                break;
                            }
                            case FieldType::UNSIGNED_INT: {
                                allValues[subField.name] =
                                    *static_cast<uint32_t *>(subFieldValuePtr);
                                break;
                            }
                            case FieldType::CHAR_ARRAY: {
                                allValues[subField.name] =
                                    *static_cast<std::string *>(
                                        subFieldValuePtr);
                                break;
                            }
                            case FieldType::FLOAT_FIXED:
                            case FieldType::FLOAT_MANTISSA: {
                                allValues[subField.name] =
                                    *static_cast<float *>(subFieldValuePtr);
                                break;
                            }
                            case FieldType::DOUBLE: {
                                allValues[subField.name] =
                                    *static_cast<double *>(subFieldValuePtr);
                                break;
                            }
                            case FieldType::BOOLEAN: {
                                allValues[subField.name] =
                                    *static_cast<bool *>(subFieldValuePtr);
                                break;
                            }
                            default:
                                throw std::runtime_error(
                                    "Unknown field type in bit field");
                        }
                    }

                    break;
                }
                default:
                    throw std::runtime_error("Unknown field type");
            }
        }
        catch (const std::exception &e) {
            allValues[field.name] = std::string("Error: ") + e.what();
        }
    }

    return allValues;
}

uint8_t *PacketParser::extractBits(const uint8_t *bitFieldData, size_t offset,
                                   size_t size) const
{
    // Calculate the number of bytes required to store the result
    size_t numBytes = (size + 7) / 8;

    // Allocate memory for the result
    uint8_t *result = (uint8_t *)std::malloc(numBytes);
    if (!result) {
        return nullptr;  // Allocation failed
    }
    std::memset(result, 0, numBytes);  // Initialize with zeros

    size_t byteIndex = offset / 8;
    size_t bitOffset = offset % 8;

    size_t bitsRemaining = size;
    size_t bytePosition = 0;

    while (bitsRemaining > 0) {
        // Read the byte from the data
        uint8_t byte = bitFieldData[byteIndex];

        // Calculate how many bits we can take from this byte
        size_t bitsToTake = std::min(bitsRemaining, 8 - bitOffset);

        // Extract the bits
        uint8_t bits =
            (byte >> (8 - bitOffset - bitsToTake)) & ((1 << bitsToTake) - 1);

        // Calculate the position to place the bits in the result buffer
        size_t resultBitOffset = (8 * bytePosition) + (8 - bitsToTake);

        // Place the bits in the result buffer
        result[bytePosition] |=
            (bits << (8 - bitsToTake - (resultBitOffset % 8)));

        // Update the number of bits remaining
        bitsRemaining -= bitsToTake;
        bitOffset = 0;  // Reset bitOffset for the next byte
        byteIndex++;    // Move to the next byte

        // Move to the next byte in the result buffer if necessary
        if (bitsRemaining > 0) {
            bytePosition++;
            if (bytePosition >= numBytes) {
                // If we've run out of space in the result buffer, we need more memory
                uint8_t *newResult =
                    (uint8_t *)std::realloc(result, bytePosition + 1);
                if (!newResult) {
                    std::free(result);
                    return nullptr;  // Allocation failed
                }
                std::memset(newResult + bytePosition, 0,
                            1);  // Initialize new byte
                result = newResult;
            }
        }
    }

    return result;
}

void PacketParser::printFieldValues(
    const std::map<std::string, FieldValue> &fieldValues)
{
    for (const auto &entry : fieldValues) {
        std::cout << entry.first << " : ";
        std::visit(
            [](const auto &value) {
                using ValueType = std::decay_t<decltype(value)>;

                if constexpr (std::is_same_v<ValueType, unsigned int>) {
                    std::cout << value;
                }
                else if constexpr (std::is_same_v<ValueType, int>) {
                    std::cout << value;
                }
                else if constexpr (std::is_same_v<ValueType, std::string>) {
                    std::cout << value;
                }
                else if constexpr (std::is_same_v<ValueType, float>) {
                    std::cout << std::fixed << std::setprecision(8) << value;
                }
                else if constexpr (std::is_same_v<ValueType, double>) {
                    std::cout << std::fixed << std::setprecision(16) << value;
                }
                else if constexpr (std::is_same_v<ValueType, bool>) {
                    std::cout << (value ? "true" : "false");
                }
                else if constexpr (std::is_same_v<ValueType, unsigned long>) {
                    std::cout << value;
                }
                else {
                    std::cout << "Unknown type";
                }
            },
            entry.second);
        std::cout << std::endl;
    }
}