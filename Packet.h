#include <cstdint>
#include <cstring>
#include "communication.h"

class Packet {
public:
    int getPsn() const;
    int getTotalPacketSum() const;
    uint8_t* getData();
    void setPsn(int psn);
    void setTotalPacketSum(int totalPacketSum);
    void setData(const void* inputData, size_t size);
    
private:
    int PSN = 0;
    int totalPacketSum = 0;
    uint8_t data[communication::PACKET_SIZE] = {0};
};
