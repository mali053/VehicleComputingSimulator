#include <algorithm>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stdexcept>
#include <variant>
#include <bitset>
#include "packet_parser.h"

using json = nlohmann::json;

FieldType PacketParser::getFieldType(const std::string &typeStr) const
{
    static const std::unordered_map<std::string, FieldType> fieldTypeMap = {
        {"unsigned_int", FieldType::UNSIGNED_INT},
        {"char_array", FieldType::CHAR_ARRAY},
        {"float_fixed", FieldType::FLOAT_FIXED},
        {"float_mantissa", FieldType::FLOAT_MANTISSA},
        {"bit_field", FieldType::BIT_FIELD},
        {"double", FieldType::DOUBLE},
        {"signed_int", FieldType::SIGNED_INT},
        {"boolean", FieldType::BOOLEAN}
    };

    auto it = fieldTypeMap.find(typeStr);
    if (it != fieldTypeMap.end()) {
        return it->second;
    }

    return FieldType::UNKNOWN;
}

PacketParser::PacketParser(const std::string &jsonFilePath)
                        : result(nullptr), numBytes(0)
{
    fieldDecoders = {
        {FieldType::UNSIGNED_INT, [this](const uint8_t* data, int size) {
            return decodeUnsignedInt(data, size);
        }},
        {FieldType::SIGNED_INT, [this](const uint8_t* data, int size) {
            return decodeSignedInt(data, size);
        }},
        {FieldType::CHAR_ARRAY, [this](const uint8_t* data, int size) {
            return decodeCharArray(data, BITS_TO_BYTES(size));
        }},
        {FieldType::FLOAT_FIXED, [this](const uint8_t* data, int size) {
            return decodeFloat(data, size);
        }},
        {FieldType::FLOAT_MANTISSA, [this](const uint8_t* data, int size) {
            return decodeFloat(data, size);
        }},
        {FieldType::DOUBLE, [this](const uint8_t* data, int size) {
            return decodeDouble(data, size);
        }},
        {FieldType::BOOLEAN, [this](const uint8_t* data, int size) {
            return decodeBoolean(data, size);
        }}
    };
    
    loadJson(jsonFilePath);
    setBuffer(nullptr);
}

PacketParser::~PacketParser()
{
        std::free(result);
}

const std::vector<Field>& PacketParser::getFields() const
{
    return fields;
}

const void* PacketParser::getBuffer() const 
{
    return _buffer;
}

void PacketParser::validateFieldSize(const std::string &type,
                                     size_t bitLength) const
{
    if ((type == "float_fixed" || type == "float_mantissa") &&
        bitLength != 32) {
        throw std::runtime_error(type + " types must be 32 bits.");
    }
    if ((type == "signed_int" || type == "unsigned_int") && bitLength != 32) {
        throw std::runtime_error(type + " must be 32 bits.");
    }
    if (type == "boolean" && bitLength != 8) {
        throw std::runtime_error("Boolean types must be 8 bits.");
    }
    if (type == "double" && bitLength != 64) {
        throw std::runtime_error("Double types must be 64 bits.");
    }
}

FieldValue PacketParser::getDefaultValueByType(const std::string &type)
{
    static const std::unordered_map<std::string, FieldValue> defaultValues = {
        {"unsigned_int", 0u},
        {"signed_int", 0},
        {"char_array", ""},
        {"boolean", false},
        {"float_fixed", 0.0f},
        {"float_mantissa", 0.0f},
        {"double", 0.0}
    };

    auto it = defaultValues.find(type);
    if (it != defaultValues.end()) {
        return it->second;
    }

    throw std::runtime_error("Unknown type for default value: " + type);
}

void PacketParser::loadJson(const std::string &jsonFilePath)
{
    std::ifstream jsonFile(jsonFilePath);
    if (!jsonFile.is_open()) {
        throw std::runtime_error("Unable to open JSON file: " + jsonFilePath + " - " + std::strerror(errno));
    }

    json jsonData;
    jsonFile >> jsonData;

    endianness = jsonData["endianness"];

    size_t currentOffset = 0;

    std::unordered_map<std::string, std::function<void(
    const json&, Field&)>> typeHandlers = {
        {"unsigned_int", [](const json& value, Field& field) {
            field.defaultValue = static_cast<uint32_t>(
                value.get<int>());
        }},
        {"signed_int", [](const json& value, Field& field) {
            field.defaultValue = static_cast<int32_t>(
                value.get<int>());
        }},
        {"char_array", [](const json& value, Field& field) {
            field.defaultValue = value.get<std::string>();
        }},
        {"boolean", [](const json& value, Field& field) {
            field.defaultValue = value.get<bool>();
        }},
        {"float_fixed", [](const json& value, Field& field) {
            field.defaultValue = static_cast<float>(
                value.get<double>());
        }},
        {"double", [](const json& value, Field& field) {
            field.defaultValue = value.get<double>();
        }},
    };

    for (const auto &fieldJson : jsonData["fields"]) {
        size_t fieldSize = fieldJson["size"];
        std::string fieldType = fieldJson["type"];
        std::string fieldName = fieldJson["name"];

        validateFieldSize(fieldType, fieldSize);

        size_t byteLength = BITS_TO_BYTES(fieldSize);
        Field field;
        field.name = fieldName;
        field.type = fieldType;
        field.size = fieldSize;
        field.offset = currentOffset;

        if (fieldJson.contains("default_value")) {
            const auto &defaultValue = fieldJson["default_value"];
                auto it = typeHandlers.find(fieldType);
                if (it != typeHandlers.end()) {
                    it->second(defaultValue, field);
                } else {
                    std::cerr << "Warning: Unknown field type '" << fieldType 
                              << "' for field '" << fieldName << "'." << std::endl;
                }
        } else if (field.type != "bit_field") {
            field.defaultValue = getDefaultValueByType(fieldType);
        }

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

                Field subField;
                subField.name = subFieldJson["name"];
                subField.type = subFieldType;
                subField.size = subFieldSize;
                subField.offset = bitFieldOffset;

                if (subFieldJson.contains("default_value")) {
                    const auto &defaultValue = fieldJson["default_value"];
                    auto it = typeHandlers.find(fieldType);
                    if (it != typeHandlers.end()) {
                        it->second(defaultValue, field);
                    } else {
                    std::cerr << "Warning: Unknown field type '" << fieldType 
                              << "' for field '" << fieldName << "'." << std::endl;
                }
                } else {
                    subField.defaultValue = getDefaultValueByType(subFieldType);
                }

                bitField.fields.push_back(subField);
                bitFieldOffset += subField.size;
            }

            bitFields.push_back(bitField);
        }

        currentOffset += byteLength;
    }
}

std::vector<Field> PacketParser::getBitFieldFields(const std::string &bitFieldName)
{
    for (const auto &bitField : bitFields) {
        if (bitField.name == bitFieldName) {
            return bitField.fields;
        }
    }

    throw std::runtime_error("BitField not found: " + bitFieldName);
}

uint32_t PacketParser::decodeUnsignedInt(const uint8_t *data,
                                         size_t bitLength) const
{
    size_t byteLength = BITS_TO_BYTES(bitLength);
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
    size_t byteLength = BITS_TO_BYTES(bitLength );
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
    size_t byteLength = BITS_TO_BYTES(bitLength);
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

FieldValue PacketParser::getFieldValue(const std::string &fieldName)
{
    if (_buffer == nullptr) {
        throw std::runtime_error("Buffer is null");
    }

    // Regular fields processing
    auto it = std::find_if(
        fields.begin(), fields.end(),
        [&fieldName](const Field &field) { return field.name == fieldName; });

    if (it != fields.end() && it->type != "bit_field") {
        const Field &field = *it;
        const uint8_t *data = static_cast<const uint8_t *>(_buffer) + field.offset;
        FieldType fieldType = getFieldType(field.type);

        auto decoderIt = fieldDecoders.find(fieldType);
        if (decoderIt != fieldDecoders.end()) {
            return decoderIt->second(data, field.size);
        } else {
            throw std::runtime_error("Unknown field type");
        }
    }

    // Bit field processing
    for (const auto &bitField : bitFields) {
        const uint8_t *bitFieldData = static_cast<const uint8_t *>(_buffer) + bitField.offset;

        auto subFieldIt = std::find_if(bitField.fields.begin(), bitField.fields.end(),
            [&fieldName](const Field &subField) {
                return subField.name == fieldName;
            });

        if (subFieldIt != bitField.fields.end()) {
            const Field &subField = *subFieldIt;

            uint8_t *extractedBits = extractBits(bitFieldData, subField.offset, subField.size);
            if (!extractedBits) {
                throw std::runtime_error("Failed to extract bits");
            }

            FieldType fieldType = getFieldType(subField.type);

            auto decoderIt = fieldDecoders.find(fieldType);
            if (decoderIt != fieldDecoders.end()) {
                return decoderIt->second(extractedBits, subField.size);
            } else {
                throw std::runtime_error("Unknown field type in bit field");
            }
        }
    }

    throw std::runtime_error("Field not found in any bit field");
}

std::map<std::string, FieldValue> PacketParser::getAllFieldValues()
{
    std::map<std::string, FieldValue> allFieldValues;

    // Retrieve regular fields
    for (const auto &field : fields) {
        if(field.type=="bit_field") 
            continue;
        const uint8_t *data = static_cast<const uint8_t *>(_buffer) + field.offset;
        allFieldValues[field.name] = getFieldValue(field.name);
    }

    // Retrieve bit field values
    for (const auto &bitField : bitFields) {
        const uint8_t *bitFieldData = static_cast<const uint8_t *>(_buffer) + bitField.offset;

        for (const auto &subField : bitField.fields) {
            auto extractedBits = extractBits(bitFieldData, subField.offset, subField.size);
            allFieldValues[subField.name] = getFieldValue(subField.name);
        }
    }

    return allFieldValues;
}

uint8_t *PacketParser::extractBits(const uint8_t *bitFieldData, 
                                    size_t offset, size_t size)
{
    size_t numBytes = BITS_TO_BYTES(size);
    uint8_t *result = (uint8_t *)std::calloc(numBytes, sizeof(uint8_t));
    
    if (!result) {
        return nullptr;
    }

    size_t byteIndex = offset / 8;
    size_t bitOffset = offset % 8;
    size_t bitsRemaining = size;
    size_t bytePosition = 0;

    while (bitsRemaining > 0) {
        uint8_t byte = bitFieldData[byteIndex];
        size_t bitsToTake = std::min(bitsRemaining, 8 - bitOffset);
        uint8_t bits = (byte >> (8 - bitOffset - bitsToTake)) & 
                       ((1 << bitsToTake) - 1);

        size_t resultBitOffset = (8 * bytePosition) + (8 - bitsToTake);
        result[bytePosition] |= (bits << (8 - bitsToTake - 
                                            (resultBitOffset % 8)));

        bitsRemaining -= bitsToTake;
        bitOffset = 0;
        byteIndex++;

        if (bitsRemaining > 0) {
            bytePosition++;
            if (bytePosition >= numBytes) {
                uint8_t *newResult = (uint8_t *)std::realloc(result, 
                    bytePosition + 1);
                if (!newResult) {
                    std::free(result);
                    return nullptr;
                }
                std::memset(newResult + bytePosition, 0, 1);
                result = newResult;
            }
        }
    }

    return result;
}

void PacketParser::setBuffer(const void *buffer) 
{
    _buffer = buffer;
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