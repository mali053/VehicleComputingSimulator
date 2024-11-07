#pragma once
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <functional>
#include <iostream>
#include "message.h"
#include "../sockets/Isocket.h"
#include "../sockets/real_socket.h"
#include <string>
#include <atomic>
#include "error_code.h"

#define PORT 8080
#define IP "127.0.0.1"

class ClientConnection
{
private:
    int clientSocket;
    sockaddr_in servAddress;
    std::atomic<bool> connected;
    std::function<void(Packet &)> passPacketCom;
    ISocket* socketInterface;
    std::thread receiveThread;

public:
    // Constructor
    ClientConnection(std::function<void(Packet &)> callback, ISocket* socketInterface = new RealSocket());

    // Requesting a connection to the server
    ErrorCode connectToServer(int id);

    // Sends the packet to the manager-sync
    ErrorCode sendPacket(Packet &packet);

    // Waits for a message and forwards it to Communication
    void receivePacket();

    // Closes the connection
    ErrorCode closeConnection();

    // Setter for passPacketCom
    void setCallback(std::function<void(Packet&)> callback);
    
    // Setter for socketInterface
    void setSocketInterface(ISocket* socketInterface);

    // For testing
    int getClientSocket();
    
    int isConnected();
    
    bool isReceiveThreadRunning();

    //Destructor
    ~ClientConnection();
};

