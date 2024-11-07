#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../include/client_connection.h"
#include "../sockets/mock_socket.h"
#include "../include/message.h"

using ::testing::_;
using ::testing::Return;

class ClientTest : public ::testing::Test {
protected:
    MockSocket mockSocket;
    ClientConnection* client;

    void SetUp() override {
        ON_CALL(mockSocket, socket(::testing::_, ::testing::_, ::testing::_))
            .WillByDefault(::testing::Return(1));
        ON_CALL(mockSocket, setsockopt(::testing::_, ::testing::_, ::testing::_, ::testing::NotNull(), ::testing::_))
            .WillByDefault(::testing::Return(0));
        ON_CALL(mockSocket, bind(::testing::_, ::testing::_, ::testing::_))
            .WillByDefault(::testing::Return(0));
        ON_CALL(mockSocket, listen(::testing::_, ::testing::_))
            .WillByDefault(::testing::Return(0));
        ON_CALL(mockSocket, connect(_, _, _))
            .WillByDefault(::testing::Return(0));
        ON_CALL(mockSocket, close(_))
            .WillByDefault(::testing::Return(0));

        ON_CALL(mockSocket, socket(_, _, _))
        .WillByDefault(Return(1)); 
    
        ON_CALL(mockSocket, setsockopt(_, _, _, _, _))
        .WillByDefault(Return(0));
    
        ON_CALL(mockSocket, connect(_, _, _))
        .WillByDefault(Return(0));
    
        ON_CALL(mockSocket, send(_, _, _, _))
        .WillByDefault(Return(44));  // ודא ששליחה מחזירה הצלחה
    
        ON_CALL(mockSocket, close(_))
        .WillByDefault(Return(0));  // close מחזיר הצלחה

        EXPECT_CALL(mockSocket, close(_)).Times(1);  // מצפה ש-close תוקרא פעם אחת


        Packet packet;
        ON_CALL(mockSocket, send(_, _, _, _))
            .WillByDefault(::testing::Return(sizeof(Packet)));

        client = new ClientConnection([](Packet &){}, &mockSocket);
    }

    void TearDown() override {
        EXPECT_CALL(mockSocket, close(_)).Times(1);
        //delete client;
    }
};

// Test Constructor
TEST_F(ClientTest, ConstructorInitializesCorrectly) {
    EXPECT_FALSE(client->isConnected()); 
    EXPECT_GT(client->getClientSocket(), 0); 
}

// Test connection success
TEST_F(ClientTest, ConnectToServerSuccess) {
    EXPECT_CALL(mockSocket, connect(_, _, _)).WillOnce(Return(0));
    ErrorCode result = client->connectToServer(1);
    EXPECT_EQ(result, ErrorCode::SUCCESS);
    EXPECT_TRUE(client->isConnected());
}

// Test connection failure (connect fails)
TEST_F(ClientTest, ConnectToServerFailureOnConnect) {
    EXPECT_CALL(mockSocket, connect(_, _, _)).WillOnce(Return(-1));
    ErrorCode result = client->connectToServer(1);
    EXPECT_EQ(result, ErrorCode::CONNECTION_FAILED);
    EXPECT_FALSE(client->isConnected());
}

// Test connection failure (socket creation fails)
TEST_F(ClientTest, ConnectToServerSocketFailure) {
    EXPECT_CALL(mockSocket, socket(_, _, _)).WillOnce(Return(-1));
    ErrorCode result = client->connectToServer(1);
    EXPECT_EQ(result, ErrorCode::SOCKET_FAILED);
    EXPECT_FALSE(client->isConnected());
}

// Test sendPacket success
TEST_F(ClientTest, SendPacketSuccess) {
    Packet packet;
    client->connectToServer(1);
    ErrorCode result = client->sendPacket(packet);
    EXPECT_EQ(result, ErrorCode::SUCCESS);
}

// Test sendPacket failure - not connected
TEST_F(ClientTest, SendPacketFailureNotConnected) {
    Packet packet;
    ErrorCode result = client->sendPacket(packet);
    EXPECT_EQ(result, ErrorCode::CONNECTION_FAILED);
}

// Test sendPacket failure - socket send fails
TEST_F(ClientTest, SendPacketFailureOnSend) {
    Packet packet;
    client->connectToServer(1);
    EXPECT_CALL(mockSocket, send(_, _, _, _)).WillOnce(Return(-1));
    ErrorCode result = client->sendPacket(packet);
    EXPECT_EQ(result, ErrorCode::SEND_FAILED);
}

// Test sendPacket partial send
TEST_F(ClientTest, SendPacketPartialSend) {
    Packet packet;
    client->connectToServer(1);
    EXPECT_CALL(mockSocket, send(_, _, _, _)).WillOnce(Return(sizeof(Packet) - 1));
    ErrorCode result = client->sendPacket(packet);
    EXPECT_EQ(result, ErrorCode::SEND_FAILED);
}

// Test receivePacket success
TEST_F(ClientTest, ReceivePacketSuccess) {
    Packet packet;
    EXPECT_CALL(mockSocket, recv(_, _, _, _)).WillOnce(Return(sizeof(Packet)));
    client->connectToServer(1);
    std::thread receiveThread(&ClientConnection::receivePacket, client);
    receiveThread.join();
    // If passPacketCom works, this could be tested, but for now we'll assume correct handling
}

// Test receivePacket failure (recv fails)
TEST_F(ClientTest, ReceivePacketFailure) {
    EXPECT_CALL(mockSocket, recv(_, _, _, _)).WillOnce(Return(-1));
    client->connectToServer(1);
    std::thread receiveThread(&ClientConnection::receivePacket, client);
    receiveThread.join();
    EXPECT_FALSE(client->isConnected());
}

// Test receivePacket no data (recv returns 0)
TEST_F(ClientTest, ReceivePacketNoData) {
    EXPECT_CALL(mockSocket, recv(_, _, _, _)).WillOnce(Return(0));
    client->connectToServer(1);
    std::thread receiveThread(&ClientConnection::receivePacket, client);
    receiveThread.join();
    EXPECT_TRUE(client->isConnected()); // Should still be connected, no data means wait for more
}

// Test closeConnection success
TEST_F(ClientTest, CloseConnectionSuccess) {
    client->connectToServer(1);
    ErrorCode result = client->closeConnection();
    EXPECT_EQ(result, ErrorCode::SUCCESS);
    EXPECT_FALSE(client->isConnected());
}

// Test closeConnection failure
TEST_F(ClientTest, CloseConnectionFailure) {
    EXPECT_CALL(mockSocket, close(_)).WillOnce(Return(-1));
    ErrorCode result = client->closeConnection();
    EXPECT_EQ(result, ErrorCode::CLOSE_FAILED);
}

// Test setCallback throws on null function
TEST_F(ClientTest, SetCallbackThrowsOnNull) {
    EXPECT_THROW(client->setCallback(nullptr), std::invalid_argument);
}

// Test setSocketInterface throws on null
TEST_F(ClientTest, SetSocketInterfaceThrowsOnNull) {
    EXPECT_THROW(client->setSocketInterface(nullptr), std::invalid_argument);
}

// Test valid socket interface setting
TEST_F(ClientTest, SetSocketInterfaceSuccess) {
    MockSocket anotherMockSocket;
    EXPECT_NO_THROW(client->setSocketInterface(&anotherMockSocket));
}

// Test connection thread starts correctly
TEST_F(ClientTest, ConnectionStartsReceiveThread) {
    client->connectToServer(1);
    EXPECT_TRUE(client->isReceiveThreadRunning());
}

// Test destructor closes connection
TEST_F(ClientTest, DestructorClosesConnection) {
    client->connectToServer(1);
    delete client; // Should close connection in destructor
    EXPECT_CALL(mockSocket, close(_)).Times(1); // Ensure close is called
}

// Test connection failure after sending a partial packet
TEST_F(ClientTest, SendPacketPartialConnectionClose) {
    Packet packet;
    client->connectToServer(1);
    EXPECT_CALL(mockSocket, send(_, _, _, _)).WillOnce(Return(sizeof(Packet) - 1));
    ErrorCode result = client->sendPacket(packet);
    EXPECT_EQ(result, ErrorCode::SEND_FAILED);
    EXPECT_FALSE(client->isConnected()); // Should close connection after failure
}
