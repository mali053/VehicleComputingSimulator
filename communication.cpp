#include "communication.h"
#include <cstring>
#include <iostream>
#include <thread>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>

#define IP_ADDRESS "127.0.0.1"
#define BACKLOG 3
const size_t communication::PACKET_SIZE = 16;

// Receive messages from a socket
void communication::receiveMessages(int socketFd)
{
    uint8_t buffer[PACKET_SIZE] = {0};
    std::cout << "before the while in receive func" << std::endl; // Before entering the loop in the receive function
    
    while (true) {
        int valread = recv(socketFd, buffer, PACKET_SIZE, 0);
        if (valread <= 0) {
            std::cerr << "Connection closed or error occurred" << std::endl; // Connection closed or an error occurred
            break;
        }

        std::cout << "Received packet: ";
        for (int i = 0; i < valread; ++i) {
            std::cout << static_cast<int>(buffer[i]) << " ";
        }
        std::cout << std::endl;
    }
}

// Send messages through a socket
void communication::sendMessage(int socketFd, void* data, size_t dataLen)
{
    uint8_t buffer[PACKET_SIZE] = {0};
    size_t offset = 0;
    
    while (offset < dataLen) {
        size_t packetLen = std::min(dataLen - offset, PACKET_SIZE);
        std::memcpy(buffer, static_cast<uint8_t*>(data) + offset, packetLen);
        int sendAns = send(socketFd, buffer, packetLen, 0);
        if(sendAns < 0){
            std::cerr << "The send failed" << std::endl; // The send failed
            break;
        }
        offset += packetLen;
    }
}

// Initialize a connection and set up listening
int communication::initConnection(int portNumber) {
    int peerPort = (portNumber == PORT1) ? PORT2 : PORT1;

    int sockFd, newSocket;
    struct sockaddr_in address, peerAddr;
    int opt = 1;
    int addrlen = sizeof(address);

    sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        perror("Socket creation error");
        return -1;
    }

    int sockopt = setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    if (sockopt) {
        perror("setsockopt");
        close(sockFd);
        return -1;
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(portNumber);

    int bindAns = bind(sockFd, (struct sockaddr*)&address, sizeof(address));
    if (bindAns < 0) {
        perror("Bind failed");
        close(sockFd);
        return -1;
    }

    int listenAns = listen(sockFd, BACKLOG);
    if (listenAns < 0) {
        perror("Listen");
        close(sockFd);
        return -1;
    }

    std::cout << "Listening on port " << portNumber << std::endl;

    memset(&peerAddr, 0, sizeof(peerAddr));
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(peerPort);

    int inetAns = inet_pton(AF_INET, IP_ADDRESS, &peerAddr.sin_addr);
    if (inetAns <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    int clientSock = -1;
    std::thread recvThread;

    if (portNumber == PORT1) {
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Waiting for the other process to listen
        clientSock = socket(AF_INET, SOCK_STREAM, 0);

        if (connect(clientSock, (struct sockaddr*)&peerAddr, sizeof(peerAddr)) < 0) {
            perror("Connection Failed");
            return -1;
        }

        newSocket = accept(sockFd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (newSocket < 0) {
            perror("Accept failed");
            return -1;
        }

        recvThread = std::thread(&communication::receiveMessages, this, newSocket);

    } else {
        newSocket = accept(sockFd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (newSocket < 0) {
            perror("Accept failed");
            return -1;
        }

        recvThread = std::thread(&communication::receiveMessages, this, newSocket);
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Waiting for the server to listen
        
        clientSock = socket(AF_INET, SOCK_STREAM, 0);
        if (connect(clientSock, (struct sockaddr*)&peerAddr, sizeof(peerAddr)) < 0) {
            perror("Connection Failed");
            return -1;
        }
    }

    recvThread.detach();
    std::cout << "Connection initialized" << std::endl;
    return clientSock;
}
