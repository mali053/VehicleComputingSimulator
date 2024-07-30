#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <cstddef>
#include <iostream>
#include <mutex>

class communication {
public:
    int initConnection();
    void sendMessage(int socketFd, void* data, size_t dataLen);
    void receiveMessages(int socketFd);
    
private:
    int setupSocket(int portNumber, int& sockFd, struct sockaddr_in& address);
    int waitForConnection(int sockFd, struct sockaddr_in& address);
    int connectToPeer(int portNumber, struct sockaddr_in& peerAddr);
    void initializeState(int& portNumber, int& peerPort);

    const int PORT1 = 8080;
    const int PORT2 = 8081;
    static const size_t PACKET_SIZE;
    static const char* STATE_FILE;
    static const char* LOCK_FILE;
    static std::mutex state_file_mutex;
};

#endif