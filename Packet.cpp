#include "Packet.h"

int Packet::getPsn() const
{
    return PSN;
}

int Packet::getTotalPacketSum() const
{
    return totalPacketSum;
}

uint8_t* Packet::getData()
{
    return data;
}

void Packet::setPsn(int psn)
{
    this->PSN = psn;
}

void Packet::setTotalPacketSum(int totalPacketSum)
{
    this->totalPacketSum = totalPacketSum;
}

void Packet::setData(const void* inputData, size_t size)
{
    std::memcpy(this->data, inputData, size);
}