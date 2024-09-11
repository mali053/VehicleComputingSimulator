#include "../include/bus_manager.h"

BusManager* BusManager::instance = nullptr;
std::mutex BusManager::managerMutex;

//Private constructor
BusManager::BusManager(std::vector<uint32_t> idShouldConnect, uint32_t limit) :server(8080, std::bind(&BusManager::receiveData, this, std::placeholders::_1))//,syncCommunication(idShouldConnect, limit)
{
    // Setup the signal handler for SIGINT
    signal(SIGINT, BusManager::signalHandler);
}

// Static function to return a singleton instance
BusManager* BusManager::getInstance(std::vector<uint32_t> idShouldConnect, uint32_t limit) {
    if (instance == nullptr) {
        // Lock the mutex to prevent multiple threads from creating instances simultaneously
        std::lock_guard<std::mutex> lock(managerMutex);
        if (instance == nullptr) {
            instance = new BusManager(idShouldConnect, limit);
        }
    }
    return instance;
}

// Sends to the server to listen for requests
ErrorCode BusManager::startConnection()
{
    
    ErrorCode isConnected = server.startConnection();
    //syncCommunication.notifyProcess()
    return isConnected;
}

// Receives the packet that arrived and checks it before sending it out
void BusManager::receiveData(Packet &p)
{
    ErrorCode res = sendToClients(p);

    // Checking the case of collision and priority in functions : checkCollision,packetPriority
    // Packet* resolvedPacket = checkCollision(*p);
    // if (resolvedPacket)
    //     server.sendToClients(*resolvedPacket);
}

// Sending according to broadcast variable
ErrorCode BusManager::sendToClients(const Packet &packet)
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

BusManager::~BusManager() {
    instance = nullptr;
}
