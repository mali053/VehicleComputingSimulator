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

#define IP_ADDRESS "127.0.0.1"
#define BACKLOG 3

// Constants for packet size and state file names

const char* communication::STATE_FILE = "comm_state3.txt";
const char* communication::LOCK_FILE = "comm_state3.lock";
std::mutex communication::state_file_mutex;

// Send acknowledgment messages
void communication::sendAck(int socketFd, AckType ackType)
{
    const char* ackMessage = (ackType == AckType::ACK) ? "ACK" : "NACK";
    send(socketFd, ackMessage, strlen(ackMessage), 0);

    // Trigger the acknowledgment callback
    if (ackCallback) {
        ackCallback(ackType);
    }
}

// Set the callback for data received
void communication::setDataHandler(std::function<void(const std::vector<uint8_t>&)> callback)
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
        int valread = recv(socketFd, &packet, sizeof(Packet), 0);
        if (valread <= 0) {
            std::cerr << "Connection closed or error occurred" << std::endl; // Connection closed or an error occurred
            break;
        }

        // Append the received data to the vector
        for (int i = 0; i < valread; ++i) {
            if (i < sizeof(packet.getData())) {
                receivedData.push_back(packet.getData()[i]);
            }
        }

        // Notify the user via callback function
        if (dataHandler) {
            dataHandler(receivedData);
        }

        // Check if all packets have been received
        if (packet.getPsn() == packet.getTotalPacketSum()) {
            sendAck(socketFd, AckType::ACK);
        }
    }
}

//Send message in a socket
void communication::sendMessage(int socketFd, void* data, size_t dataLen)
{
    Packet packet;
    int psn = 0;
    size_t offset = 0;
    size_t totalPackets = (dataLen + PACKET_SIZE - 1) / PACKET_SIZE;

    while (offset < dataLen) {
        size_t packetLen = std::min(dataLen - offset, PACKET_SIZE);
        packet.setData(static_cast<uint8_t*>(data) + offset, packetLen);
        packet.setPsn(psn++);
        packet.setTotalPacketSum(totalPackets - 1);

        int sendAns = send(socketFd, &packet, sizeof(Packet), 0);
        if(sendAns < 0){
            std::cerr << "The send failed" << std::endl; // The send failed
            break;
        }
        offset += packetLen;
    }

    // Wait for acknowledgment
    char buffer[4];
    int valread = recv(socketFd, buffer, sizeof(buffer), 0);
    if (valread > 0) {
        buffer[valread] = '\0'; // Null-terminate the received string
        AckType receivedAck = (strcmp(buffer, "ACK") == 0) ? AckType::ACK : AckType::NACK;

        // Trigger the acknowledgment callback
        if (ackCallback) {
            ackCallback(receivedAck);
        }
    } else {
        std::cerr << "Failed to receive acknowledgment" << std::endl;
    }
}

// Initialize the state based on the presence of the state file
void communication::initializeState(int& portNumber, int& peerPort)
{
    std::lock_guard<std::mutex> lock(state_file_mutex);
    std::ifstream infile(STATE_FILE);

    if (!infile) {  // If the file doesn't exist - means this is the first process
        portNumber = PORT2;
        peerPort = PORT1;
        std::ofstream outfile(STATE_FILE);
        outfile << "initialized";
        outfile.close();  
        std::ofstream lockFile(LOCK_FILE); 
        lockFile.close();
    } else {  // If the file already exists - means this is the second process
        portNumber = PORT1;
        peerPort = PORT2;
    }
    infile.close();
}

// Set up a socket for listening
int communication::setupSocket(int portNumber, int& sockFd, struct sockaddr_in& address)
{
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
    return 0;
}

// Wait for an incoming connection
int communication::waitForConnection(int sockFd, struct sockaddr_in& address)
{
    int newSocket;
    int addrlen = sizeof(address);
    newSocket = accept(sockFd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    if (newSocket < 0) {
        perror("Accept failed");
        return -1;
    }
    return newSocket;
}

// Connect to a peer socket
int communication::connectToPeer(int portNumber, struct sockaddr_in& peerAddr)
{
    int clientSock = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSock < 0) {
        perror("Socket creation error");
        return -1;
    }

    int inetAns = inet_pton(AF_INET, IP_ADDRESS, &peerAddr.sin_addr);
    if (inetAns <= 0) {
        perror("Invalid address/ Address not supported");
        return -1;
    }

    int connectAns = connect(clientSock, (struct sockaddr*)&peerAddr, sizeof(peerAddr));
    if (connectAns < 0) {
        perror("Connection Failed");
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
        if (clientSock < 0) return -1; 

        // Wait for an incoming connection from PORT2
        newSocket = waitForConnection(sockFd, address);
        if (newSocket < 0) return -1; 
        
        // Start a separate thread to handle incoming messages on the new socket
        recvThread = std::thread(&communication::receiveMessages, this, newSocket);

    } else {
        // Wait for an incoming connection from PORT1
        newSocket = waitForConnection(sockFd, address);
        if (newSocket < 0) return -1; 
        
        // Start a separate thread to handle incoming messages on the new socket
        recvThread = std::thread(&communication::receiveMessages, this, newSocket);
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Waiting for the server to listen

        clientSock = connectToPeer(portNumber, peerAddr);
        if (clientSock < 0) return -1; 

        // Clean up: remove state and lock files as they are no longer needed
        std::remove(STATE_FILE);
        std::remove(LOCK_FILE);
    }

    recvThread.detach();
    return clientSock;
}