#ifndef __PACKET_PARSER_H__
#define __PACKET_PARSER_H__

#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <variant>
#include <functional>
#include <vector>

#define BITS_TO_BYTES(bits) (((bits) + 7) / 8)

using FieldValue = std::variant<unsigned int, std::string, float,
                                 double, int, bool>;

enum class FieldType {
    UNSIGNED_INT,
    SIGNED_INT,
    CHAR_ARRAY,
    FLOAT_FIXED,
    FLOAT_MANTISSA,
    BIT_FIELD,
    DOUBLE,
    BOOLEAN,
    UNKNOWN
};

struct Field {
    std::string name;
    std::string type;
    FieldValue defaultValue;
    size_t size;
    size_t offset;
};

struct BitField {
    std::string name;
    size_t size;
    size_t offset;
    std::vector<Field> fields;
};

class PacketParser {
public:
    PacketParser(const std::string &jsonFilePath);
    FieldValue getFieldValue(const std::string &fieldName);
    std::map<std::string, FieldValue> getAllFieldValues();
    const std::vector<Field> &getFields() const;
    const void* getBuffer() const;
    FieldType getFieldType(const std::string &typeName) const;
    void printFieldValues(const std::map<std::string, FieldValue> &fieldValues);
    void setBuffer(const void *buffer);
    std::vector<Field> getBitFieldFields(const std::string &bitFieldName);
    FieldValue getDefaultValueByType(const std::string &type);
    ~PacketParser();

protected:
    uint8_t *extractBits(const uint8_t *bitFieldData, size_t offset,
                         size_t size);

private:
    std::vector<Field> fields;
    std::vector<BitField> bitFields;
    std::string endianness;
    const void *_buffer;
    uint8_t *result;
    size_t numBytes;
    std::map<FieldType, std::function<FieldValue(const uint8_t*, int)>> fieldDecoders;

    void validateFieldSize(const std::string &type, size_t bitLength) const;
    void loadJson(const std::string &jsonFilePath);
    uint32_t decodeUnsignedInt(const uint8_t *data, size_t bitLength) const;
    int32_t decodeSignedInt(const uint8_t *data, size_t bitLength) const;
    std::string decodeCharArray(const uint8_t *data, size_t byteLength) const;
    float decodeFloat(const uint8_t *data, size_t bitLength) const;
    double decodeDouble(const uint8_t *data, size_t bitLength) const;
    bool decodeBoolean(const uint8_t *data, size_t bitLength) const;
};

#endif  // PACKET_PARSER_H