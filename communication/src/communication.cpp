#include "../include/communication.h"
#include <future>

Communication* Communication::instance = nullptr;

// Constructor
Communication::Communication(uint32_t id, void (*passDataCallback)(void *)) : client(id, std::bind(&Communication::receivePacket, this, std::placeholders::_1)), passData(passDataCallback), id(id)
{
    instance = this;
    // Setup the signal handler for SIGINT
    signal(SIGINT, Communication::signalHandler);
}

// Sends the client to connect to server
void Communication::startConnection()
{
    client.connectToServer();
}

// Sends a message sync
int Communication::sendMessage(void *data, size_t dataSize, uint32_t destID, uint32_t srcID, bool isBroadcast)
{
    // Creating a message and dividing it into packets
    Message msg(srcID, data, dataSize, isBroadcast, destID);
    
    for (auto &packet : msg.getPackets()) {
        int res = client.sendPacket(packet);
        if(res < 0)
            return res;
    }

    return 0;  
}

// Sends a message Async
void Communication::sendMessageAsync(void *data, size_t dataSize, uint32_t destID, uint32_t srcID, std::function<void(int)> sendCallback, bool isBroadcast)
{
    std::promise<int> resultPromise;
    std::future<int> resultFuture = resultPromise.get_future();

    std::thread([this, data, dataSize, destID, srcID, isBroadcast, &resultPromise]() {
        int res = this->sendMessage(data, dataSize, destID, srcID, isBroadcast);
        resultPromise.set_value(res);
    }).detach();
    
    int res = resultFuture.get();
    sendCallback(res);
}

// Accepts the packet from the client and checks..
void Communication::receivePacket(Packet &p)
{
    if (checkDestId(p)) {
        if (validCRC(p))
            handlePacket(p);
        else
            handleError();
    }
}

// Checks if the packet is intended for him
bool Communication::checkDestId(Packet &p)
{
    return p.header.isBroadcast || p.header.DestID == this->id;
}

// Checks if the data is currect
bool Communication::validCRC(Packet &p)
{
    return p.header.CRC == p.calculateCRC(p.data, p.header.DLC);
}

// Receives the packet and adds it to the message
void Communication::handlePacket(Packet &p)
{
    // Send acknowledgment according to CAN bus
    // client.sendPacket(hadArrived());
    addPacketToMessage(p);
}

// Implement error handling according to CAN bus
void Communication::handleError()
{
    // Handle error cases according to CAN bus
}

// Implement arrival confirmation according to the CAN bus
Packet Communication::hadArrived()
{
    Packet ack;
    // Construct and return acknowledgment packet
    return ack;
}

// Adding the packet to the complete message
void Communication::addPacketToMessage(Packet &p)
{
    // messageId may have to change according to the CAN bus
    std::string messageId = std::to_string(p.header.SrcID) + "-" + std::to_string(p.header.DestID);
    
    // If the message already exists, we will add the packet
    if (receivedMessages.find(messageId) != receivedMessages.end()) {
        receivedMessages[messageId].addPacket(p);
    } else {
        // If the message does not exist, we will create a new message
        Message msg(p.header.TPS);
        msg.addPacket(p);
        receivedMessages[messageId] = msg;
    }

    // If the message is complete, we pass the data to the passData function
    if (receivedMessages[messageId].isComplete()) {
        void *completeData = receivedMessages[messageId].completeData();
        passData(completeData);
        receivedMessages.erase(messageId); // Removing the message once completed
    }
}

// Static method to handle SIGINT signal
void Communication::signalHandler(int signum)
{
    std::cout << "Interrupt signal (" << signum << ") received.\n";
    if (instance) {
        instance->client.closeConnection();  // Call the closeConnection method
    }
    exit(signum);
}

//Destructor
Communication::~Communication() {}