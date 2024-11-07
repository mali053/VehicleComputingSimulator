#pragma once
#include <mutex>
#include <utility>
#include "server_connection.h"
#include <iostream>

class BusManager
{
private:
    ServerConnection server;

    // Singleton instance
    static BusManager* instance;
    static std::mutex managerMutex;
    //SyncCommunication syncCommunication;
    
    // Sending according to broadcast variable
    ErrorCode sendToClients(const Packet &packet);

    // Private constructor
    BusManager(std::vector<uint32_t> idShouldConnect, uint32_t limit);

public:
    //Static function to return a singleton instance
    static BusManager* getInstance(std::vector<uint32_t> idShouldConnect, uint32_t limit);

    // Sends to the server to listen for requests
    ErrorCode startConnection();

    // Receives the packet that arrived and checks it before sending it out
    void receiveData(Packet &p);

    // Implementation according to the conflict management of the CAN bus protocol
    Packet checkCollision(Packet &currentPacket);

    // Implement a priority check according to the CAN bus
    Packet packetPriority(Packet &a, Packet &b);

    // Static method to handle SIGINT signal
    static void signalHandler(int signum);

    ~BusManager();
};