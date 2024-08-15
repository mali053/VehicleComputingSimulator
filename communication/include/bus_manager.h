#pragma once
#include <mutex>
#include <utility>
#include "server.h"
#include <iostream>

class BusManager
{
private:

    Server server;
   
    // Sending according to broadcast variable
    int sendToClients(const Packet &packet);

public:
    // constructor
    BusManager();

    // Sends to the server to listen for requests
    int startConnection();

    // Receives the packet that arrived and checks it before sending it out
    void receiveData(void *data);

    // Implementation according to the conflict management of the CAN bus protocol
    Packet checkCollision(Packet &currentPacket);

    // Implement a priority check according to the CAN bus
    Packet packetPriority(Packet &a, Packet &b);

    ~BusManager();
};