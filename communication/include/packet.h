#pragma once
#include <vector>
#include <cstring>
#include <cstdint>
#include <ctime>
#include <cstdlib>
#include <unordered_map>
#include <iostream>
#include <bitset>
#include <cstdint>
#include <iomanip>
#include <sstream>
#define SIZE_PACKET 8
class Packet
{
public:
    // Packet header containing various metadata fields
    struct Header
    {
        uint32_t ID;      // Message ID
        uint32_t PSN;     // Packet Sequence Number
        uint32_t TPS;     // Total Packet Sum
        uint32_t SrcID;   // Source ID
        uint32_t DestID;  // Destination ID
        uint8_t DLC;          // Data Length Code (0-8 bits)
        uint16_t CRC;     // Cyclic Redundancy Check for error detection
        int timestamp;    // Timestamp field
        bool isBroadcast; // True for broadcast, false for unicas
        bool passive;
        bool RTR;
    } header;

    void *data[SIZE_PACKET];

    // Default constructor for Packet.
    Packet() = default;

    // Constructor for sending message
    Packet(uint32_t id, uint32_t psn, uint32_t tps, uint32_t srcID, uint32_t destID, void *data, uint8_t dlc, bool isBroadcast, bool RTR = false, bool passive = false);

    // Constructor for receiving message
    Packet(uint32_t id);

    // Calculate CRC for the given data and length
    uint16_t calculateCRC(const void *data, size_t length);

    // A function to convert the data to hexa (logger)
    std::string pointerToHex(const void *ptr, size_t size) const;
};
