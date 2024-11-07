#include "../include/client_connection.h"

// Constructor
ClientConnection::ClientConnection(std::function<void(Packet &)> callback, ISocket* socketInterface): connected(false){
        setCallback(callback);
        setSocketInterface(socketInterface);
}

// Requesting a connection to the server
ErrorCode ClientConnection::connectToServer(int id)
{
    clientSocket = socketInterface->socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        return ErrorCode::SOCKET_FAILED;
    }

    servAddress.sin_family = AF_INET;
    servAddress.sin_port = htons(PORT);
    inet_pton(AF_INET, IP, &servAddress.sin_addr);

    int connectRes = socketInterface->connect(clientSocket, (struct sockaddr *)&servAddress, sizeof(servAddress));
    if (connectRes < 0) {
        socketInterface->close(clientSocket);
        return ErrorCode::CONNECTION_FAILED;
    }

    Packet packet(id);
    ssize_t bytesSent = socketInterface->send(clientSocket, &packet, sizeof(Packet), 0);
    if (bytesSent < sizeof(Packet)) {
        socketInterface->close(clientSocket);
        return ErrorCode::SEND_FAILED;
    }
    
    connected = true;
    receiveThread = std::thread(&ClientConnection::receivePacket, this);
    receiveThread.detach();

    return ErrorCode::SUCCESS;
}

// Sends the packet to the manager-sync
ErrorCode ClientConnection::sendPacket(Packet &packet)
{
    //If send executed before start
    if (!connected)
        return ErrorCode::CONNECTION_FAILED;
        
    ssize_t bytesSent = socketInterface->send(clientSocket, &packet, sizeof(Packet), 0);
    if (bytesSent==0) {
        closeConnection();
        return ErrorCode::CONNECTION_FAILED;
    }
        
    if (bytesSent<0)
        return ErrorCode::SEND_FAILED;
        
    return ErrorCode::SUCCESS;
}

// Waits for a message and forwards it to Communication
void ClientConnection::receivePacket()
{
    while (connected) {
        Packet packet;
        int valread = socketInterface->recv(clientSocket, &packet, sizeof(Packet), 0);
        if (valread==0)
            break;

        if (valread<0)
            continue;

        passPacketCom(packet);
    }

    closeConnection();
}

// Closes the connection
ErrorCode ClientConnection::closeConnection()
{
    if (connected) {
        int socketInterfaceRes = socketInterface->close(clientSocket);
        if(socketInterfaceRes < 0)
            return ErrorCode::CLOSE_FAILED;
        connected = false;
    }
    return ErrorCode::SUCCESS;  
}

// Setter for passPacketCom
void ClientConnection::setCallback(std::function<void(Packet&)> callback) {
    if (!callback)
        throw std::invalid_argument("Callback function cannot be null");
    
    passPacketCom = callback;
}

// Setter for socketInterface
void ClientConnection::setSocketInterface(ISocket* socketInterface) {
    if (!socketInterface)
        throw std::invalid_argument("Socket interface cannot be null");
    
    this->socketInterface = socketInterface;
}

// For testing
int ClientConnection::getClientSocket()
{
    return clientSocket;
}

int ClientConnection::isConnected()
{
    return connected;
}

bool ClientConnection::isReceiveThreadRunning()
{
    return false;
}

//Destructor
ClientConnection::~ClientConnection()
{
    closeConnection();
    delete socketInterface;
}