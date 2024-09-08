#include "../include/message.h"

// Constructor for sending message
Message::Message(uint32_t srcID, void *data, int dlc, bool isBroadcast, uint32_t destID)
{
    size_t size = dlc;
    uint32_t tps = (size + 7) / 8; // Calculate the number of packets needed
    for (uint32_t i = 0; i < tps; ++i) {
        uint8_t packetData[8];
        size_t copySize = std::min(size - i * 8, (size_t)8); // Determine how much data to copy for each packet
        std::memcpy(packetData, (uint8_t *)data + i * 8, copySize);
        uint32_t id = srcID + destID;
        packets.emplace_back(id, i, tps, srcID, destID, packetData, copySize, isBroadcast, false, false);
    }
}

// Constructor for receiving message
Message::Message(uint32_t tps)
{
    this->tps = tps;
}

// Add a packet to the received message
bool Message::addPacket(const Packet &p)
{
    // Implementation according to the CAN BUS
    
    // For further testing
    // if (p.header.PSN >= packets.size()) {
    //     return false;
    // }

    packets.push_back(p);
    return true;
}

// Check if the message is complete
bool Message::isComplete() const
{
    return packets.size() == tps;
}

// Get the complete data of the message
void *Message::completeData() const
{
    size_t totalSize = (tps - 1) * 8 + packets.back().header.DLC;
    void *data = malloc(totalSize);
    for (const auto &packet : packets) {
        std::memcpy((uint8_t *)data + packet.header.PSN * 8, packet.data, packet.header.DLC);
    }
    return data;
}

// Get the packets of the message
std::vector<Packet> &Message::getPackets()
{
    return packets;
}
