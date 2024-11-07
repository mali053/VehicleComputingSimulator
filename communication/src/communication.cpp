#include "../include/communication.h"
#include <future>

Communication* Communication::instance = nullptr;

// Constructor
Communication::Communication(uint32_t id, void (*passDataCallback)(uint32_t, void *)) : 
    client(std::bind(&Communication::receivePacket, this, std::placeholders::_1))
{
    setId(id);
    setPassDataCallback(passDataCallback);

    instance = this;

    auto signalResult = signal(SIGINT, Communication::signalHandler);
    if (signalResult == SIG_ERR)
        throw std::runtime_error("Failed to set signal handler for SIGINT");
}

// Sends the client to connect to server
ErrorCode Communication::startConnection()
{
    //Waiting for manager
    //syncCommunication.isManagerRunning()
    ErrorCode isConnected = client.connectToServer(id);
    //Increases the shared memory and blocks the process - if not all are connected
    //syncCommunication.registerProcess()
    return isConnected;
}

// Sends a message sync
ErrorCode Communication::sendMessage(void *data, size_t dataSize, uint32_t destID, uint32_t srcID, bool isBroadcast)
{
    if (dataSize == 0)
        return ErrorCode::INVALID_DATA_SIZE;

    if (data == nullptr)
        return ErrorCode::INVALID_DATA;

    if (!client.isConnected())
        return ErrorCode::CONNECTION_FAILED;

    Message msg(srcID, data, dataSize, isBroadcast, destID);
    
    //Sending the message to logger
    RealSocket::log.logMessage(logger::LogLevel::INFO,std::to_string(srcID),std::to_string(destID),"Complete message:" + msg.getPackets().at(0).pointerToHex(data, dataSize));
    
    for (auto &packet : msg.getPackets()) {
        ErrorCode res = client.sendPacket(packet);
        if (res != ErrorCode::SUCCESS)
            return res;
    }

    return ErrorCode::SUCCESS;  
}

// Sends a message Async
void Communication::sendMessageAsync(void *data, size_t dataSize, uint32_t destID, uint32_t srcID, std::function<void(ErrorCode)> sendCallback, bool isBroadcast)
{
    std::promise<ErrorCode> resultPromise;
    std::future<ErrorCode> resultFuture = resultPromise.get_future();

    std::thread([this, data, dataSize, destID, srcID, isBroadcast, &resultPromise]() {
        ErrorCode res = this->sendMessage(data, dataSize, destID, srcID, isBroadcast);
        resultPromise.set_value(res);
    }).detach();
    
    ErrorCode res = resultFuture.get();
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
    std::string messageId = std::to_string(p.header.ID);
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
        passData(p.header.SrcID, completeData);
        receivedMessages.erase(messageId); // Removing the message once completed
    }
}

// Static method to handle SIGINT signal
void Communication::signalHandler(int signum)
{
    if (instance)
        instance->client.closeConnection();
    
    exit(signum);
}

void Communication::setId(uint32_t newId)
{
    id = newId;
}

void Communication::setPassDataCallback(void (*callback)(uint32_t, void *))
{
    if (callback == nullptr)
        throw std::invalid_argument("Invalid callback function: passDataCallback cannot be null");
    
    passData = callback;
}

//Destructor
Communication::~Communication() {
    instance = nullptr;
}