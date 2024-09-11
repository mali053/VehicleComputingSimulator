#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../include/server_connection.h"
#include "../sockets/mock_socket.h"
#include "../include/message.h"

using ::testing::_;
using ::testing::Return;

class ServerTest : public ::testing::Test {
protected:
    MockSocket* mockSocket;
    ServerConnection* server;
    int testPort = 8080;
    Packet testPacket;

    void SetUp() override {
        mockSocket = new MockSocket();
        server = new ServerConnection(testPort, [](Packet& packet) {}, mockSocket);
    }

    void TearDown() override {
        delete server;
    }
};

// Test for successful startConnection
TEST_F(ServerTest, StartConnection_Success) {
    EXPECT_CALL(*mockSocket, socket(AF_INET, SOCK_STREAM, 0))
        .WillOnce(Return(3));  // Return a valid socket fd

    EXPECT_CALL(*mockSocket, setsockopt(3, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, _, sizeof(int)))
        .WillOnce(Return(0));

    EXPECT_CALL(*mockSocket, bind(3, _, sizeof(sockaddr_in)))
        .WillOnce(Return(0));

    EXPECT_CALL(*mockSocket, listen(3, 5))
        .WillOnce(Return(0));

    ErrorCode result = server->startConnection();
    EXPECT_EQ(result, ErrorCode::SUCCESS);
}

// Test for socket creation failure
TEST_F(ServerTest, StartConnection_SocketFailed) {
    EXPECT_CALL(*mockSocket, socket(AF_INET, SOCK_STREAM, 0))
        .WillOnce(Return(-1));  // Simulate socket creation failure

    ErrorCode result = server->startConnection();
    EXPECT_EQ(result, ErrorCode::SOCKET_FAILED);
}

// Test for bind failure
TEST_F(ServerTest, StartConnection_BindFailed) {
    EXPECT_CALL(*mockSocket, socket(AF_INET, SOCK_STREAM, 0))
        .WillOnce(Return(3));  // Return a valid socket fd

    EXPECT_CALL(*mockSocket, setsockopt(3, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, _, sizeof(int)))
        .WillOnce(Return(0));

    EXPECT_CALL(*mockSocket, bind(3, _, sizeof(sockaddr_in)))
        .WillOnce(Return(-1));  // Simulate bind failure


    EXPECT_CALL(*mockSocket, close(3)).Times(1);  // Expect close call due to failure

    ErrorCode result = server->startConnection();
    EXPECT_EQ(result, ErrorCode::BIND_FAILED);
}

// Test for listen failure
TEST_F(ServerTest, StartConnection_ListenFailed) {
    EXPECT_CALL(*mockSocket, socket(AF_INET, SOCK_STREAM, 0))
        .WillOnce(Return(3));  // Return a valid socket fd

    EXPECT_CALL(*mockSocket, setsockopt(3, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, _, sizeof(int)))
        .WillOnce(Return(0));

    EXPECT_CALL(*mockSocket, bind(3, _, sizeof(sockaddr_in)))
        .WillOnce(Return(0));

    EXPECT_CALL(*mockSocket, listen(3, 5))
        .WillOnce(Return(-1));  // Simulate listen failure

    EXPECT_CALL(*mockSocket, close(3)).Times(1);  // Expect close call due to failure

    ErrorCode result = server->startConnection();
    EXPECT_EQ(result, ErrorCode::LISTEN_FAILED);
}

// Test for successful sendDestination
TEST_F(ServerTest, SendDestination_Success) {
    int clientSocket = 5;
    testPacket.header.DestID = 1;

    // Simulate successful send operation
    EXPECT_CALL(*mockSocket, send(clientSocket, &testPacket, sizeof(Packet), 0))
        .WillOnce(Return(sizeof(Packet)));

    {
        std::lock_guard<std::mutex> lock(*server->getIDMapMutex());
        (*server->getClientIDMap())[clientSocket] = testPacket.header.DestID;
    }

    ErrorCode result = server->sendDestination(testPacket);
    EXPECT_EQ(result, ErrorCode::SUCCESS);
}

// Test for sendDestination failure
TEST_F(ServerTest, SendDestination_Failed) {
    int clientSocket = 5;
    testPacket.header.DestID = 1;

    // Simulate send failure
    EXPECT_CALL(*mockSocket, send(clientSocket, &testPacket, sizeof(Packet), 0))
        .WillOnce(Return(0));

    {
        std::lock_guard<std::mutex> lock(*server->getIDMapMutex());
        (*server->getClientIDMap())[clientSocket] = testPacket.header.DestID;
    }

    ErrorCode result = server->sendDestination(testPacket);
    EXPECT_EQ(result, ErrorCode::SEND_FAILED);
}

// Test for sendDestination failure
TEST_F(ServerTest, SendDestination_Connection_Failed) {
    int clientSocket = 5;
    testPacket.header.DestID = 1;

    // Simulate send failure
    EXPECT_CALL(*mockSocket, send(clientSocket, &testPacket, sizeof(Packet), 0))
        .WillOnce(Return(-1));

    {
        std::lock_guard<std::mutex> lock(*server->getIDMapMutex());
        (*server->getClientIDMap())[clientSocket] = testPacket.header.DestID;
    }

    ErrorCode result = server->sendDestination(testPacket);
    EXPECT_EQ(result, ErrorCode::CONNECTION_FAILED);
}

// Test for sending message to an invalid client ID
TEST_F(ServerTest, SendDestination_InvalidClientID) {
    testPacket.header.DestID = 999;  // Invalid client ID
    ErrorCode result = server->sendDestination(testPacket);
    EXPECT_EQ(result, ErrorCode::INVALID_CLIENT_ID);  // Ensure failure for invalid client ID
}

// Test for send failure when sending to a client
TEST_F(ServerTest, SendDestination_SendFailed) {
    testPacket.header.DestID = 1;  // Valid client ID
    int clientSocket = 3;
    EXPECT_CALL(*mockSocket, send(clientSocket, _, sizeof(Packet), 0))
        .WillOnce(Return(0));  // Simulate send failure

    // EXPECT_CALL(*mockSocket, close(clientSocket));  // Close socket on failure

    {
        std::lock_guard<std::mutex> lock(*server->getIDMapMutex());
        (*server->getClientIDMap())[clientSocket] = testPacket.header.DestID;  // Map client to ID
    }

    ErrorCode result = server->sendDestination(testPacket);
    EXPECT_EQ(result, ErrorCode::SEND_FAILED);  // Ensure correct error code on send failure
}

// Test for closing an invalid socket
TEST_F(ServerTest, CloseInvalidSocket) {
    int invalidSocket = -1;

    EXPECT_CALL(*mockSocket, close(invalidSocket))
        .Times(0);  // No close call since socket is invalid

    server->stopServer();
    EXPECT_EQ(server->testGetClientSocketByID(999), -1);  // Ensure client does not exist
}

// Test for successful broadcast
TEST_F(ServerTest, SendBroadcast_Success) {
    int clientSocket1 = 3;
    int clientSocket2 = 4;

    // Simulate successful broadcast to two clients
    EXPECT_CALL(*mockSocket, send(clientSocket1, _, sizeof(Packet), 0))
        .WillOnce(Return(sizeof(Packet)));  // Success for client 1

    EXPECT_CALL(*mockSocket, send(clientSocket2, _, sizeof(Packet), 0))
        .WillOnce(Return(sizeof(Packet)));  // Success for client 2

    {
        std::lock_guard<std::mutex> lock(*server->getSocketMutex());
        (*server->getSockets()).push_back(clientSocket1);  // Add client 1
        (*server->getSockets()).push_back(clientSocket2);  // Add client 2
    }

    ErrorCode result = server->sendBroadcast(testPacket);
    EXPECT_EQ(result, ErrorCode::SUCCESS);  // Ensure broadcast was successful
}

// Test for broadcast failure
TEST_F(ServerTest, SendBroadcast_SendFailed) {
    int clientSocket1 = 3;
    int clientSocket2 = 4;

    EXPECT_CALL(*mockSocket, send(clientSocket1, _, sizeof(Packet), 0))
        .WillOnce(Return(sizeof(Packet)));  // Success for client 1

    EXPECT_CALL(*mockSocket, send(clientSocket2, _, sizeof(Packet), 0))
        .WillOnce(Return(-1));  // Failure for client 2

    //EXPECT_CALL(*mockSocket, close(clientSocket2));  // Close socket on failure

    {
        std::lock_guard<std::mutex> lock(*server->getSocketMutex());
        (*server->getSockets()).push_back(clientSocket1);  // Add client 1
        (*server->getSockets()).push_back(clientSocket2);  // Add client 2
    }

    ErrorCode result = server->sendBroadcast(testPacket);
    EXPECT_EQ(result, ErrorCode::CONNECTION_FAILED);  // Ensure broadcast failure is handled
}

// Test for handling client disconnection during message reception
TEST_F(ServerTest, HandleClient_Disconnection) {
    int clientSocket = 5;

    EXPECT_CALL(*mockSocket, recv(clientSocket, _, sizeof(Packet), 0))
        .WillOnce(Return(0));  // Simulate client disconnection

    //EXPECT_CALL(*mockSocket, close(clientSocket));  // Close socket for disconnected client

    server->testHandleClient(clientSocket);
    std::vector<int>* sockets = server->getSockets();

    {
        std::lock_guard<std::mutex> lock(*server->getSocketMutex());
        EXPECT_EQ(std::find(sockets->begin(), sockets->end(), clientSocket), sockets->end());
    }
}

// Test for receive failure from a client
TEST_F(ServerTest, HandleClient_ReceiveFailed) {
    int clientSocket = 5;

    EXPECT_CALL(*mockSocket, recv(clientSocket, _, sizeof(Packet), 0))
        .WillOnce(Return(-1));  // Simulate receive failure

    //EXPECT_CALL(*mockSocket, close(clientSocket));  // Close socket on failure

    server->testHandleClient(clientSocket);

    std::vector<int>* sockets = server->getSockets();
    {
        std::lock_guard<std::mutex> lock(*server->getSocketMutex());
        EXPECT_EQ(std::find(sockets->begin(), sockets->end(), clientSocket), sockets->end());
    }
}