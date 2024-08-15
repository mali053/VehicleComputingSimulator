
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/client.h"
#include "../sockets/mock_socket.h"
#include "../src/message.h"

using ::testing::_;
using ::testing::Return;

class ClientTest : public ::testing::Test {
protected:
    MockSocket mockSocket;
    Client *client;

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
        Packet packet;
        ON_CALL(mockSocket, send(_, _, _, _))
            .WillByDefault(::testing::Return(sizeof(Packet)));
  
        client= new Client(1, [](Packet&){}, &mockSocket);      
    }
    void TearDown() override {
       // delete client;
    }
};

TEST_F(ClientTest, ConstructorCreatesSocket) {
    EXPECT_GT(client->getClientSocket(), 0);
}

TEST_F(ClientTest, ConnectToServerSuccess) {
    int result = client->connectToServer();
    EXPECT_EQ(result, 0);
}

TEST_F(ClientTest, ConnectToServerFailure) {
    EXPECT_CALL(mockSocket, connect(_, _, _))
        .WillOnce(Return(-1));
    int result = client->connectToServer();
    EXPECT_EQ(result, -1);
}

TEST_F(ClientTest, SendPacketSuccess) {
    Packet packet;
    client->connectToServer();
    int result = client->sendPacket(packet);
    EXPECT_EQ(result, 0);
}

TEST_F(ClientTest, SendPacketFailureNotConnected) {
    Packet packet;
    int result = client->sendPacket(packet);
    EXPECT_EQ(result, -1);
}

TEST_F(ClientTest, CloseConnection) {
    client->closeConnection();
    EXPECT_FALSE(client->isConnected());
}
