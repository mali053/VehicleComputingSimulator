#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <cstddef>
#include <iostream>

class communication {
public:
    int initConnection(int portNumber);
    void sendMessage(int socketFd, void* data, size_t dataLen);
    void receiveMessages(int socketFd);

private:
    const int PORT1 = 8080;
    const int PORT2 = 8081;
    static const size_t PACKET_SIZE;
};

#endif
