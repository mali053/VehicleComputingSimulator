#include "../include/server.h"

// Constructor
Server::Server(int port, std::function<void(void *)> callback, ISocket* socketInterface)
    : port(port), receiveDataCallback(callback), running(false), socketInterface(socketInterface) {}

// Initializes the listening socket
int Server::startConnection()
{
    // Create socket TCP
    serverSocket = socketInterface->socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        return -1;
    }

    // Setting the socket to allow reuse of address and port
    int opt = 1;
    int setSockOptRes = socketInterface->setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    if (setSockOptRes) {
        socketInterface->close(serverSocket);
        return -1;
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    int bindRes = socketInterface->bind(serverSocket, (struct sockaddr *)&address, sizeof(address));
    if (bindRes < 0) {
        socketInterface->close(serverSocket);
    }

    int lisRes = socketInterface->listen(serverSocket, 5);
    if (lisRes < 0) {
        socketInterface->close(serverSocket);
        return -1;
    }
    
    running = true;
    mainThread = std::thread(&Server::startThread, this);
    mainThread.detach();

    return 0;
}

// Starts listening for connection requests
void Server::startThread()
{
    while (running) {
        int clientSocket = socketInterface->accept(serverSocket, nullptr, nullptr);
        if (clientSocket < 0) {
            return;
        }
        
        // Opens a new thread for handleClient - listening to messages from the process
        clientThreads.emplace_back(&Server::handleClient, this, clientSocket);
    }
}

// Implementation according to the CAN BUS
bool Server::isValidId(uint32_t id)
{
    return true;
}

// Closes the sockets and the threads
void Server::stopServer()
{
    running = false;
    socketInterface->close(serverSocket);

    std::lock_guard<std::mutex> lock(socketMutex);
    for (int sock : sockets)
        socketInterface->close(sock);

    for (auto &th : clientThreads)
        if (th.joinable())
            th.join();

    if(mainThread.joinable())
        mainThread.join();
}

// Runs in a thread for each process - waits for a message and forwards it to the manager
void Server::handleClient(int clientSocket)
{
    Packet packet;
    int valread = socketInterface->recv(clientSocket, &packet, sizeof(Packet), 0);
    if (valread <= 0)
        return;
    
    uint32_t clientID = packet.header.SrcID;
    if(!isValidId(clientID))
        return;
    {
        std::lock_guard<std::mutex> lock(IDMapMutex);
        clientIDMap[clientSocket] = clientID;
    }

    {
        std::lock_guard<std::mutex> lock(socketMutex);
        sockets.push_back(clientSocket);
    }

    while (true) {
        int valread = socketInterface->recv(clientSocket, &packet, sizeof(Packet), 0);
        if (valread <= 0)
            break;
        
        receiveDataCallback(&packet);
    }
    
    // If the process is no longer connected
    std::lock_guard<std::mutex> lock(socketMutex);
    auto it = std::find(sockets.begin(), sockets.end(), clientSocket);
    if (it != sockets.end())
        sockets.erase(it);
}

// Returns the sockets ID
int Server::getClientSocketByID(uint32_t destID)
{
    std::lock_guard<std::mutex> lock(IDMapMutex);
    for (const auto &client : clientIDMap)
        if (client.second == destID)
            return client.first;

    return -1;
}

// Sends the message to destination
int Server::sendDestination(const Packet &packet)
{
    int targetSocket = getClientSocketByID(packet.header.DestID);
    if (targetSocket == -1)
        return -1;
    
    ssize_t bytesSent = socketInterface->send(targetSocket, &packet, sizeof(Packet), 0);
    if (bytesSent < sizeof(Packet))
        return -1;

    return 0;
}

// Sends the message to all connected processes - broadcast
int Server::sendBroadcast(const Packet &packet)
{
    std::lock_guard<std::mutex> lock(socketMutex);
    for (int sock : sockets) {
        ssize_t bytesSent = socketInterface->send(sock, &packet, sizeof(Packet), 0);
        if (bytesSent < sizeof(Packet))
            return -1;
    }

    return 0;
}

// For testing
int Server::getServerSocket()
{
    return serverSocket;
}

int Server::isRunning()
{
    return running;
}

// Destructor
Server::~Server()
{
    stopServer();
    delete socketInterface;
}