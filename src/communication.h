#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <cstddef>
#include <iostream>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <functional>
#include "ISocket.h"

enum class AckType {
    ACK,
    NACK
};

class communication {
public:
    communication(ISocket* socketInterface);

    virtual int initConnection();
    virtual void sendMessage(int socketFd, void* data, size_t dataLen);
    virtual void receiveMessages(int socketFd);
    virtual void setDataHandler(std::function<void(const std::vector<uint8_t>&)> callback);
    virtual void setAckCallback(std::function<void(AckType)> callback);
    static constexpr size_t PACKET_SIZE = 16;
    friend class CommunicationTest;

    virtual int setupSocket(int portNumber, int& sockFd, struct sockaddr_in& address);
    virtual int waitForConnection(int sockFd, struct sockaddr_in& address);
    virtual int connectToPeer(int portNumber, struct sockaddr_in& peerAddr);
    virtual void initializeState(int& portNumber, int& peerPort);
    virtual void sendAck(int socketFd, AckType ackType);
    static void logMessage(const std::string& src, const std::string& dst, const std::string& message);
    static std::string getLogFileName();

    const int PORT1 = 8080;
    const int PORT2 = 8081;
    static const char* STATE_FILE;
    static const char* LOCK_FILE;
    static std::mutex state_file_mutex;
    std::vector<uint8_t> dataReceived; 
    std::function<void(const std::vector<uint8_t>&)> dataHandler;
    std::function<void(AckType)> ackCallback;

private:
    ISocket* socketInterface;
};

#endif // COMMUNICATION_H
