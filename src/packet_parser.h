#ifndef PACKET_PARSER_H
#define PACKET_PARSER_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

class PacketParser {
public:
    PacketParser(const std::string& jsonFilePath);
    void parseBuffer(const void* buffer);

private:
    struct Field {
        std::string name;
        std::string type;
        size_t size;
        std::string endianness;
    };

    std::vector<Field> fields;
    void loadJson(const std::string& jsonFilePath);
};

#endif // PACKET_PARSER_H
