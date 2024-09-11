#include "../include/message.h"

// Constructor for sending message
Message::Message(uint32_t srcID, void *data, int dlc, bool isBroadcast, uint32_t destID)
{
    size_t size = dlc;
    uint32_t tps = (size + SIZE_PACKET-1) / SIZE_PACKET; // Calculate the number of packets needed
    for (uint32_t i = 0; i < tps; ++i) {
        uint8_t packetData[SIZE_PACKET];
        size_t copySize = std::min(size - i * SIZE_PACKET, (size_t)SIZE_PACKET); // Determine how much data to copy for each packet
        std::memcpy(packetData, (uint8_t *)data + i * SIZE_PACKET, copySize);
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
    size_t totalSize = (tps - 1) * SIZE_PACKET + packets.back().header.DLC;
    void *data = malloc(totalSize);
    for (const auto &packet : packets) {
        std::memcpy(static_cast<char*>(data) + packet.header.PSN * SIZE_PACKET, packet.data, packet.header.DLC);
    }
    return data;
}

// Get the packets of the message
std::vector<Packet> &Message::getPackets()
{
    return packets;
}
