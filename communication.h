#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <cstddef>
#include <iostream>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <functional>

// Enum for acknowledgment types
enum class AckType {
    ACK,
    NACK
};

class communication {
public:
    int initConnection();
    void sendMessage(int socketFd, void* data, size_t dataLen);
    void receiveMessages(int socketFd);
    void setDataHandler(std::function<void(const std::vector<uint8_t>&)> callback);
    void setAckCallback(std::function<void(AckType)> callback);
    static constexpr size_t PACKET_SIZE = 16;

private:
    int setupSocket(int portNumber, int& sockFd, struct sockaddr_in& address);
    int waitForConnection(int sockFd, struct sockaddr_in& address);
    int connectToPeer(int portNumber, struct sockaddr_in& peerAddr);
    void initializeState(int& portNumber, int& peerPort);
    void sendAck(int socketFd, AckType ackType);

    const int PORT1 = 8080;
    const int PORT2 = 8081;
    static const char* STATE_FILE;
    static const char* LOCK_FILE;
    static std::mutex state_file_mutex;
    std::vector<uint8_t> dataReceived; 
    std::function<void(const std::vector<uint8_t>&)> dataHandler;
    std::function<void(AckType)> ackCallback;
};

#endif