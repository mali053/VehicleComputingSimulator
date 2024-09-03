#include "../include/bus_manager.h"

BusManager* BusManager::instance = nullptr;

// constructor
BusManager::BusManager() : server(8080, std::bind(&BusManager::receiveData, this, std::placeholders::_1))
{
    instance = this;
    // Setup the signal handler for SIGINT
    signal(SIGINT, BusManager::signalHandler);
}

// Sends to the server to listen for requests
int BusManager::startConnection()
{
    return server.startConnection();
}

// Receives the packet that arrived and checks it before sending it out
void BusManager::receiveData(void *data)
{
    Packet *p = static_cast<Packet *>(data);
    int res = sendToClients(*p);
    // Checking the case of collision and priority in functions : checkCollision,packetPriority
    // Packet* resolvedPacket = checkCollision(*p);
    // if (resolvedPacket)
    //     server.sendToClients(*resolvedPacket);
}

// Sending according to broadcast variable
int BusManager::sendToClients(const Packet &packet)
{
    if(packet.header.isBroadcast)
        return server.sendBroadcast(packet);
    return server.sendDestination(packet);
}

// Implement according to the conflict management of the CAN bus protocol
Packet BusManager::checkCollision(Packet &currentPacket)
{
    return currentPacket;
}

// Implement a priority check according to the CAN bus
Packet BusManager::packetPriority(Packet &a, Packet &b)
{
    return (a.header.SrcID < b.header.SrcID) ? a : b;
}

// Static method to handle SIGINT signal
void BusManager::signalHandler(int signum)
{
    if (instance) {
        instance->server.stopServer();  // Call the stopServer method
    }
    exit(signum);
}
BusManager::~BusManager() {}
