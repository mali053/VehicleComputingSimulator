#include "communication.h"
#include "Packet.h"
#include <cstring>
#include <iostream>
#include <thread>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <vector>
#include <functional>
#include <iomanip>
#include <chrono>
#include <sstream>
#include <cerrno>

#define IP_ADDRESS "127.0.0.1"
#define BACKLOG 3

// Constants for packet size and state file names
const char *communication::STATE_FILE = "../../comm_state3.txt";
const char *communication::LOCK_FILE = "../../comm_state3.lock";
std::mutex communication::state_file_mutex;

// Function to generate a timestamped log file name
std::string communication::getLogFileName()
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time);

    std::ostringstream oss;
    oss << "../../" << std::put_time(&tm, "%Y_%m_%d_%H_%M_%S") << "_communication.log";
    return oss.str();
}

communication::communication(ISocket *socketInterface) : socketInterface(socketInterface) {}

void communication::logMessage(const std::string &src, const std::string &dst, const std::string &message)
{
    std::string logFileName = getLogFileName();
    std::ofstream logFile(logFileName, std::ios_base::app);
    if (!logFile) {
        std::cerr << "[ERROR] Failed to open log file" << std::endl;
        return;
    }
    
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&time);
    logFile << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "] "
            << "SRC: " << src << " DST: " << dst << " " << message << std::endl;
}

// Send acknowledgment messages
void communication::sendAck(int socketFd, AckType ackType)
{
    const char *ackMessage = (ackType == AckType::ACK) ? "ACK" : "NACK";
    socketInterface->send(socketFd, ackMessage, strlen(ackMessage), 0);
}

// Set the callback for data received
void communication::setDataHandler(std::function<void(const std::vector<uint8_t> &)> callback)
{
    dataHandler = callback;
}

// Set the callback for acknowledgment received
void communication::setAckCallback(std::function<void(AckType)> callback)
{
    ackCallback = callback;
}

// Receive messages from a socket
void communication::receiveMessages(int socketFd)
{
    Packet packet;
    std::vector<uint8_t> receivedData;
    while (true) {
        int valread = socketInterface->recv(socketFd, &packet, sizeof(Packet), 0);
        std::string receivePacketForLog = "";

        // Append the received data to the vector
        for (int i = 0; i < PACKET_SIZE; ++i) {
            receivedData.push_back(packet.getData()[i]);
            receivePacketForLog += packet.getData()[i];
        }
        logMessage("Server", "Client", "[INFO] " + receivePacketForLog);

        // Notify the user via callback function
        if (dataHandler) {
            dataHandler(receivedData);
        }

        receivedData.clear();

        // Check if all packets have been received
        if (packet.getPsn() == packet.getTotalPacketSum()) {
            sendAck(socketFd, AckType::ACK);
        }
    }
}

// Send message in a socket
void communication::sendMessage(int socketFd, void *data, size_t dataLen)
{
    if (data == nullptr || dataLen == 0) {
        if (ackCallback) {
            ackCallback(AckType::NACK);
        }
        return;
    }

    Packet packet;
    int psn = 0;
    size_t offset = 0;
    size_t totalPackets = (dataLen + PACKET_SIZE - 1) / PACKET_SIZE;

    while (offset < dataLen) {
        size_t packetLen = std::min(dataLen - offset, PACKET_SIZE);
        packet.setData(static_cast<uint8_t *>(data) + offset, packetLen);
        packet.setPsn(psn++);
        packet.setTotalPacketSum(totalPackets - 1);

        int sendAns = socketInterface->send(socketFd, &packet, sizeof(Packet), 0);
        if (sendAns < 0) {
            break;
        }

        offset += packetLen;
        packet.resetData();
    }
    // Wait for acknowledgment
    char buffer[4];
    int valread = socketInterface->recv(socketFd, buffer, sizeof(buffer), 0);
    if (valread > 0) {
        buffer[valread] = '\0'; // Null-terminate the received string
        AckType receivedAck = (strcmp(buffer, "ACK") == 0) ? AckType::ACK : AckType::NACK;
        logMessage("Client", "Server", "[INFO] " + (std::string)buffer);
        // Trigger the acknowledgment callback
        if (ackCallback) {
            ackCallback(receivedAck);
        }
    }
}

// Initialize the state based on the presence of the state file
void communication::initializeState(int &portNumber, int &peerPort)
{
    std::lock_guard<std::mutex> lock(state_file_mutex);
    std::ifstream infile(STATE_FILE);

    if (!infile) { 
        // If the file doesn't exist - means this is the first process
        portNumber = PORT2;
        peerPort = PORT1;
        std::ofstream outfile(STATE_FILE);
        outfile << "initialized";
        outfile.close();
        std::ofstream lockFile(LOCK_FILE);
        lockFile.close();
    }
    else { 
        // If the file already exists - means this is the second process
        portNumber = PORT1;
        peerPort = PORT2;
    }
    infile.close();
}

// Set up a socket for listening
int communication::setupSocket(int portNumber, int &sockFd, struct sockaddr_in &address)
{
    int opt = 1;
    int addrlen = sizeof(address);
    sockFd = socketInterface->socket(AF_INET, SOCK_STREAM, 0);
    if (sockFd < 0) {
        return -1;
    }

    int sockopt = socketInterface->setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
    if (sockopt < 0) {
        return -1;
    }

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(portNumber);

    int bindAns = socketInterface->bind(sockFd, (struct sockaddr *)&address, sizeof(address));
    if (bindAns < 0) {
        return -1;
    }

    int listenAns = socketInterface->listen(sockFd, BACKLOG);
    if (listenAns < 0) {
        return -1;
    }
    std::cout << "Listening on port " << portNumber << std::endl;
    return 0;
}

// Wait for an incoming connection
int communication::waitForConnection(int sockFd, struct sockaddr_in &address)
{
    int newSocket;
    int addrlen = sizeof(address);
    newSocket = socketInterface->accept(sockFd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (newSocket < 0) {
        return -1;
    }
    return newSocket;
}

// Connect to a peer socket
int communication::connectToPeer(int portNumber, struct sockaddr_in &peerAddr)
{
    int clientSock = socketInterface->socket(AF_INET, SOCK_STREAM, 0);
    if (clientSock < 0) {
        return -1;
    }

    int inetAns = inet_pton(AF_INET, IP_ADDRESS, &peerAddr.sin_addr);
    if (inetAns <= 0) {
        logMessage("Client", "Server", "[ERROR] Invalid address/ Address not supported: " + std::string(strerror(errno)));
        return -1;
    }

    int connectAns = socketInterface->connect(clientSock, (struct sockaddr *)&peerAddr, sizeof(peerAddr));
    if (connectAns < 0) {
        return -1;
    }

    return clientSock;
}

// Initialize the connection
int communication::initConnection()
{
    int portNumber, peerPort;
    initializeState(portNumber, peerPort);
    int sockFd, newSocket;
    struct sockaddr_in address, peerAddr;

    int setupSocketAns = setupSocket(portNumber, sockFd, address);
    if (setupSocketAns < 0) {
        return -1;
    }

    memset(&peerAddr, 0, sizeof(peerAddr));
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(peerPort);
    int clientSock = -1;
    std::thread recvThread;

    if (portNumber == PORT1) {
        // Wait briefly to ensure the second process (PORT2) starts listening
        std::this_thread::sleep_for(std::chrono::seconds(1));

        clientSock = connectToPeer(portNumber, peerAddr);
        if (clientSock < 0) {
            logMessage(std::to_string(portNumber), std::to_string(peerPort), "[ERROR] connect To Peer failed" + std::string(strerror(errno)));
            return -1;
        }

        logMessage(std::to_string(portNumber), std::to_string(peerPort), "[INFO] connected");

        // Wait for an incoming connection from PORT2
        newSocket = waitForConnection(sockFd, address);
        if (newSocket < 0) {
            logMessage(std::to_string(peerPort), std::to_string(portNumber), "[ERROR] wait For Connection failed" + std::string(strerror(errno)));
            return -1;
        }

        logMessage(std::to_string(peerPort), std::to_string(portNumber), "[INFO] connected");

        // Start a separate thread to handle incoming messages on the new socket
        recvThread = std::thread(&communication::receiveMessages, this, newSocket);
    }
    else {
        // Wait for an incoming connection from PORT1
        newSocket = waitForConnection(sockFd, address);
        if (newSocket < 0) {
            logMessage(std::to_string(peerPort), std::to_string(portNumber), "[ERROR] wait For Connection failed" + std::string(strerror(errno)));
            return -1;
        }

        logMessage(std::to_string(peerPort), std::to_string(portNumber), "[INFO] connected");
        // Start a separate thread to handle incoming messages on the new socket
        recvThread = std::thread(&communication::receiveMessages, this, newSocket);
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Waiting for the server to listen

        clientSock = connectToPeer(portNumber, peerAddr);
        if (clientSock < 0) {
            logMessage(std::to_string(portNumber), std::to_string(peerPort), "[ERROR] wait For Connection failed" + std::string(strerror(errno)));
            return -1;
        }

        logMessage(std::to_string(portNumber), std::to_string(peerPort), "[INFO] connected");

        // Clean up: remove state and lock files as they are no longer needed
        std::remove(STATE_FILE);
        std::remove(LOCK_FILE);
    }
    recvThread.detach();
    return clientSock;
}
