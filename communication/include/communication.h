#pragma once
#include <unordered_map>
#include "client.h"

class Communication
{
private:
    Client client;
    std::unordered_map<std::string, Message> receivedMessages;
    void (*passData)(void *); 
    uint32_t id;
    // A static variable that holds an instance of the class
    static Communication* instance;

public:
    // Constructor
    Communication(uint32_t id, void (*passDataCallback)(void *));
    
    // Sends the client to connect to server
    void startConnection();
    
    // Sends a message to manager
    int sendMessage(void *data, size_t dataSize, uint32_t destID, uint32_t srcID, bool isBroadcast);
    
    // Sends a message to manager - Async
    void sendMessageAsync(void *data, size_t dataSize, uint32_t destID, uint32_t srcID, std::function<void(int)> passSend, bool isBroadcast);

    // Accepts the packet from the client and checks..
    void receivePacket(Packet &p);
    
    // Checks if the packet is intended for him
    bool checkDestId(Packet &p);
    
    // Checks if the Packet is currect
    bool validCRC(Packet &p);
    
    // Receives the packet and adds it to the message
    void handlePacket(Packet &p);
    
    // Implement error handling according to CAN bus
    void handleError();
    
    // Implement arrival confirmation according to the CAN bus
    Packet hadArrived();
    
    // Adding the packet to the complete message
    void addPacketToMessage(Packet &p);

    // Static method to handle SIGINT signal
    static void signalHandler(int signum);
    
    //Destructor
    ~Communication();
};