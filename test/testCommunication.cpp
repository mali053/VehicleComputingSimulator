#include "communication.h"
#include "Packet.h"
#include "../sockets/mockSocket.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <thread>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>
#include <functional>
#include <iomanip>
#include <arpa/inet.h>
#include <string>

#define IP_ADDRESS "127.0.0.1"
#define PORT_NUMBER 8080
#define PEER_PORT 8081
// Test fixture class
class CommunicationTest : public ::testing::Test {
protected:
    MockSocket mockSocket;
    communication comm{&mockSocket};
    int portNumber = 8080;
    int sockFd;
    struct sockaddr_in address;
    std::vector<uint8_t> receivedData;

    void SetUp() override {
        // Set up default behavior for mock methods
        ON_CALL(mockSocket, socket(::testing::_, ::testing::_, ::testing::_))
            .WillByDefault(::testing::Return(3));
        ON_CALL(mockSocket, setsockopt(::testing::_, ::testing::_, ::testing::_, ::testing::NotNull(), ::testing::_))
            .WillByDefault(::testing::Return(0));
        ON_CALL(mockSocket, bind(::testing::_, ::testing::_, ::testing::_))
            .WillByDefault(::testing::Return(0));
        ON_CALL(mockSocket, listen(::testing::_, ::testing::_))
            .WillByDefault(::testing::Return(0));
    }

    void TearDown() override {
        // Clean up state and lock files
        std::remove(communication::STATE_FILE);
        std::remove(communication::LOCK_FILE);
    }

    void receiveMessagesCallback(const std::vector<uint8_t>& data) {
        receivedData = data;
    }

    void ackCallback(AckType ackType) {
        // Handle acknowledgment callback if needed
    }
};

// Test when state file does not exist
TEST_F(CommunicationTest, InitializeState_FileDoesNotExist) {
    std::remove(communication::STATE_FILE);
    std::remove(communication::LOCK_FILE);

    int portNumber, peerPort;
    comm.initializeState(portNumber, peerPort);

    std::ifstream stateFile(communication::STATE_FILE);
    std::ifstream lockFile(communication::LOCK_FILE);

    EXPECT_TRUE(stateFile.is_open());
    EXPECT_TRUE(lockFile.is_open());
    EXPECT_EQ(portNumber, PEER_PORT);
    EXPECT_EQ(peerPort, PORT_NUMBER);
}

// Test when state file already exists
TEST_F(CommunicationTest, InitializeState_FileExists) {
    std::ofstream stateFile(communication::STATE_FILE);
    stateFile << "initialized";
    stateFile.close();

    std::ofstream lockFile(communication::LOCK_FILE);
    lockFile.close();

    int portNumber, peerPort;
    comm.initializeState(portNumber, peerPort);

    EXPECT_EQ(portNumber, PORT_NUMBER);
    EXPECT_EQ(peerPort, PEER_PORT);
}

// Test successful setup
TEST_F(CommunicationTest, SetupSocket_Success) {
    EXPECT_CALL(mockSocket, socket(AF_INET, SOCK_STREAM, 0))
        .WillOnce(::testing::Return(3));
    EXPECT_CALL(mockSocket, setsockopt(3, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, ::testing::NotNull(), sizeof(int)))
        .WillOnce(::testing::Return(0));
    EXPECT_CALL(mockSocket, bind(3, ::testing::NotNull(), sizeof(address)))
        .WillOnce(::testing::Return(0));
    EXPECT_CALL(mockSocket, listen(3, 3))
        .WillOnce(::testing::Return(0));

    int result = comm.setupSocket(portNumber, sockFd, address);

    EXPECT_EQ(result, 0);
    EXPECT_EQ(sockFd, 3);
}

// Test failed socket creation
TEST_F(CommunicationTest, SetupSocket_FailedSocketCreation) {
    EXPECT_CALL(mockSocket, socket(AF_INET, SOCK_STREAM, 0))
        .WillOnce(::testing::Return(-1));

    int result = comm.setupSocket(portNumber, sockFd, address);

    EXPECT_EQ(result, -1);
}

// Test failed setsockopt
TEST_F(CommunicationTest, SetupSocket_FailedSetsockopt) {
    EXPECT_CALL(mockSocket, socket(AF_INET, SOCK_STREAM, 0))
        .WillOnce(::testing::Return(3));
    EXPECT_CALL(mockSocket, setsockopt(3, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, ::testing::NotNull(), sizeof(int)))
        .WillOnce(::testing::Return(-1));

    int result = comm.setupSocket(portNumber, sockFd, address);

    EXPECT_EQ(result, -1);
}

// Test failed bind
TEST_F(CommunicationTest, SetupSocket_FailedBind) {
    EXPECT_CALL(mockSocket, socket(AF_INET, SOCK_STREAM, 0))
        .WillOnce(::testing::Return(3));
    EXPECT_CALL(mockSocket, setsockopt(3, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, ::testing::NotNull(), sizeof(int)))
        .WillOnce(::testing::Return(0));
    EXPECT_CALL(mockSocket, bind(3, ::testing::NotNull(), sizeof(address)))
        .WillOnce(::testing::Return(-1));

    int result = comm.setupSocket(portNumber, sockFd, address);

    EXPECT_EQ(result, -1);
}

// Test failed listen
TEST_F(CommunicationTest, SetupSocket_FailedListen) {
    EXPECT_CALL(mockSocket, socket(AF_INET, SOCK_STREAM, 0))
        .WillOnce(::testing::Return(3));
    EXPECT_CALL(mockSocket, setsockopt(3, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, ::testing::NotNull(), sizeof(int)))
        .WillOnce(::testing::Return(0));
    EXPECT_CALL(mockSocket, bind(3, ::testing::NotNull(), sizeof(address)))
        .WillOnce(::testing::Return(0));
    EXPECT_CALL(mockSocket, listen(3, 3))
        .WillOnce(::testing::Return(-1));

    int result = comm.setupSocket(portNumber, sockFd, address);

    EXPECT_EQ(result, -1);
}

// Test sending messages
TEST_F(CommunicationTest, SendMessage) {
    EXPECT_CALL(mockSocket, send(::testing::_, ::testing::NotNull(), ::testing::_, 0))
        .WillOnce(::testing::Return(0));
    EXPECT_CALL(mockSocket, recv(::testing::_, ::testing::NotNull(), ::testing::_, 0))
        .WillOnce(::testing::Return(0));

    std::vector<uint8_t> data = {1, 2, 3, 4, 5};
    comm.setAckCallback([this](AckType ackType) {
        EXPECT_EQ(ackType, AckType::ACK);
        
    });

    comm.sendMessage(sockFd, data.data(), data.size());
}

// Test sending empty data
TEST_F(CommunicationTest, SendMessages_EmptyData) {
    EXPECT_CALL(mockSocket, send(::testing::_, ::testing::NotNull(), ::testing::_, 0))
        .Times(0);
    EXPECT_CALL(mockSocket, recv(::testing::_, ::testing::NotNull(), ::testing::_, 0))
        .Times(0);


    std::vector<uint8_t> data;
    comm.setAckCallback([this](AckType ackType) {
        EXPECT_EQ(ackType, AckType::NACK);
    });

    comm.sendMessage(sockFd, data.data(), data.size());
}

// Test sending data with multiple packets
TEST_F(CommunicationTest, SendMessages_MultiplePackets) {
    EXPECT_CALL(mockSocket, send(::testing::_, ::testing::NotNull(), ::testing::_, 0))
        .WillRepeatedly(::testing::Return(0));
     EXPECT_CALL(mockSocket, recv(::testing::_, ::testing::NotNull(), ::testing::_, 0))
        .WillOnce(::testing::Return(0));

    std::vector<uint8_t> data(200, 1); // Data size larger than packet size
    comm.setAckCallback([this](AckType ackType) {
        EXPECT_EQ(ackType, AckType::ACK);
    });

    comm.sendMessage(sockFd, data.data(), data.size());
}

// Test for `setDataReceivedCallback`
TEST_F(CommunicationTest, SetDataReceivedCallback) {
    // Define a callback to test
    auto callback = [this](const std::vector<uint8_t>& data) {
        EXPECT_EQ(data, std::vector<uint8_t>({1, 2, 3, 4, 5}));
    };

    comm.setDataHandler(callback);

    // Simulate receiving data
    std::vector<uint8_t> data = {1, 2, 3, 4, 5};
    comm.dataHandler(data); // Invoke the callback directly
}

// Test for `waitForConnection` with successful connection
TEST_F(CommunicationTest, WaitForConnection_Success) {
    EXPECT_CALL(mockSocket, accept(::testing::_, ::testing::NotNull(), ::testing::_))
        .WillOnce(::testing::Return(5));

    struct sockaddr_in address;
    int newSocket = comm.waitForConnection(sockFd, address);

    EXPECT_EQ(newSocket, 5);
}

// Test for `initConnection` with successful initialization
TEST_F(CommunicationTest, InitConnection_Success) {
    EXPECT_CALL(mockSocket, socket(AF_INET, SOCK_STREAM, 0))
        .WillRepeatedly(::testing::Return(6));
    EXPECT_CALL(mockSocket, connect(6, ::testing::NotNull(), sizeof(sockaddr_in)))
        .WillOnce(::testing::Return(0));
    EXPECT_CALL(mockSocket, accept(::testing::_, ::testing::NotNull(), ::testing::_))
        .WillOnce(::testing::Return(7));


    // Set up the necessary files
    std::ofstream stateFile(communication::STATE_FILE);
    stateFile << "initialized";
    stateFile.close();

    std::ofstream lockFile(communication::LOCK_FILE);
    lockFile.close();

    int clientSock = comm.initConnection();

    EXPECT_NE(clientSock, -1);
}

// Test for `sendAck` with ACK
TEST_F(CommunicationTest, SendAck_ACK) {
    EXPECT_CALL(mockSocket, send(::testing::_, ::testing::NotNull(), ::testing::_, 0))
        .WillOnce(::testing::Return(0));

    comm.sendAck(sockFd, AckType::ACK);
}

// Test for `sendAck` with NACK
TEST_F(CommunicationTest, SendAck_NACK) {
    EXPECT_CALL(mockSocket, send(::testing::_, ::testing::NotNull(), ::testing::_, 0))
        .WillOnce(::testing::Return(0));

    comm.sendAck(sockFd, AckType::NACK);
}

// Test `logMessage` 
TEST_F(CommunicationTest, LogToFile) {
    const char* filename = "log.txt";
    std::ofstream logFile(filename);
    if (!logFile.is_open()) {
        FAIL() << "Failed to open log file";
    }

    // Redirect std::clog to log file
    std::streambuf* orig_clog_buf = std::clog.rdbuf();
    std::clog.rdbuf(logFile.rdbuf());

    // Generate a log message
    std::clog << "Test log message" << std::endl;

    // Restore original std::clog buffer
    std::clog.rdbuf(orig_clog_buf);

    logFile.close();

    // Read back the log file and verify its contents
    std::ifstream inputFile(filename);
    ASSERT_TRUE(inputFile.is_open());

    std::string logContent((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    inputFile.close();

    ASSERT_TRUE(logContent.find("Test log message") != std::string::npos) << "Log content: " << logContent;

    std::remove(filename);
}

// Test for `ConectToPeer` with FailedSocketCreation
TEST_F(CommunicationTest, ConnectToPeer_FailedSocketCreation) {
    EXPECT_CALL(mockSocket, socket(AF_INET, SOCK_STREAM, 0))
        .WillOnce(::testing::Return(-1));

    struct sockaddr_in peerAddr;
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(PEER_PORT);
    peerAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    int result = comm.connectToPeer(PEER_PORT, peerAddr);

    EXPECT_EQ(result, -1);
}

// Test for `ConectToPeer` with FailedConnect
TEST_F(CommunicationTest, ConnectToPeer_FailedConnect) {
    EXPECT_CALL(mockSocket, socket(AF_INET, SOCK_STREAM, 0))
        .WillOnce(::testing::Return(6));
    EXPECT_CALL(mockSocket, connect(6, ::testing::NotNull(), sizeof(sockaddr_in)))
        .WillOnce(::testing::Return(-1));

    struct sockaddr_in peerAddr;
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(PEER_PORT);
    peerAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    int result = comm.connectToPeer(PORT_NUMBER, peerAddr);

    EXPECT_EQ(result, -1);
}

// Test for `ConectToPeer` with success
TEST_F(CommunicationTest, ConnectToPeer_Success) {
    EXPECT_CALL(mockSocket, socket(AF_INET, SOCK_STREAM, 0))
        .WillOnce(::testing::Return(6));

    EXPECT_CALL(mockSocket, connect(6, ::testing::NotNull(), sizeof(sockaddr_in)))
        .WillOnce(::testing::Return(0));

    struct sockaddr_in peerAddr;
    peerAddr.sin_family = AF_INET;
    peerAddr.sin_port = htons(PEER_PORT);
    int result = comm.connectToPeer(PORT_NUMBER, peerAddr);

    EXPECT_EQ(result, 6);
}