#pragma once
#include <vector>
#include <thread>
#include <future>
#include <mutex>
#include <netinet/in.h>
#include <unistd.h>
#include <functional>
#include <map>
#include "message.h"
#include "../sockets/Isocket.h"
#include "../sockets/real_socket.h"

class Server
{
private:
    int serverSocket;
    sockaddr_in address;
    int port;
    bool running;
    std::thread mainThread;
    std::vector<std::thread> clientThreads;
    std::vector<int> sockets;
    std::mutex socketMutex;
    std::function<void(void *)> receiveDataCallback;
    std::map<int, uint32_t> clientIDMap;
    std::mutex IDMapMutex;
    ISocket* socketInterface;

    // Starts listening for connection requests
    void startThread();

    // Implementation according to the CAN BUS
    bool isValidId(uint32_t id);
    
    // Runs in a thread for each process - waits for a message and forwards it to the manager
    void handleClient(int clientSocket);

    // Returns the sockets ID
    int getClientSocketByID(uint32_t destID);

public:
    // Constructor
    Server(int port, std::function<void(void *)> callback, ISocket* socketInterface = new RealSocket());
    
    // Initializes the listening socket
    int startConnection();
    
    // Closes the sockets and the threads
    void stopServer();

    // Sends the message to all connected processes - broadcast
    int sendBroadcast(const Packet &packet);

    // Sends the message to destination
    int sendDestination(const Packet &packet);
    
    // For testing
    int getServerSocket();

    int isRunning();
    
    // Destructor
     ~Server();
};