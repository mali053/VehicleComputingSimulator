#pragma once
#include <vector>
#include <cstring>
#include <cstdint>
#include <ctime>
#include <cstdlib>
#include <unordered_map>
#include <iostream>
#include "packet.h"

class Message
{
private:
    std::vector<Packet> packets;
    uint32_t tps;
                  
public:
    // Default
    Message() = default;

    // Constructor for sending message
    Message(uint32_t srcID, void *data, int dlc, bool isBroadcast, uint32_t destID = 0xFFFF);
    
    // Constructor for receiving message
    Message(uint32_t tps);

    // Add a packet to the received message
    bool addPacket(const Packet &p);

    // Check if the message is complete
    bool isComplete() const;

    // Get the complete data of the message
    void *completeData() const;

    // Get the packets of the message
    std::vector<Packet> &getPackets();
};