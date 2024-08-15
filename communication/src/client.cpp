#include "../include/client.h"

// Constructor
Client::Client(uint32_t id, std::function<void(Packet &)> callback, ISocket* socketInterface)
    : id(id), passPacketCom(callback), connected(false), socketInterface(socketInterface) {}

// Requesting a connection to the server
int Client::connectToServer()
{
    clientSocket = socketInterface->socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        return -1;
    }

    servAddress.sin_family = AF_INET;
    servAddress.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &servAddress.sin_addr);

    int connectRes = socketInterface->connect(clientSocket, (struct sockaddr *)&servAddress, sizeof(servAddress));
    if (connectRes < 0) {
        return -1;
    }

    Packet packet(id);
    ssize_t bytesSent = socketInterface->send(clientSocket, &packet, sizeof(Packet), 0);
    if (bytesSent < sizeof(Packet)) {
        return -1;
    }
    
    connected = true;
    receiveThread = std::thread(&Client::receivePacket, this);
    receiveThread.detach();
    return 0;
}

// Sends the packet to the manager-sync
int Client::sendPacket(Packet &packet)
{
    //If send executed before start
    if (!connected)
        return -1;
        
    ssize_t bytesSent = socketInterface->send(clientSocket, &packet, sizeof(Packet), 0);
    if (bytesSent < sizeof(Packet)) {
        return -1;
    }
    
    return 0;
}

// Waits for a message and forwards it to Communication
void Client::receivePacket()
{
    Packet packet;
    while (connected) {
        int valread = socketInterface->recv(clientSocket, &packet, sizeof(Packet), 0);
        if (valread <= 0) {
            break;
        }
        passPacketCom(packet);
    }
}

// Closes the connection
void Client::closeConnection()
{
    //implement - Notify the manager about disconnection
    connected = false;
    socketInterface->close(clientSocket);
    if(receiveThread.joinable())
        receiveThread.join();
}

//For testing
int Client::getClientSocket()
{
    return clientSocket;
}

int Client::isConnected()
{
    return connected;
}

bool Client::isReceiveThreadRunning()
{
    return false;
}

//Destructor
Client::~Client()
{
    closeConnection();
    delete socketInterface;
}