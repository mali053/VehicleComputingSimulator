
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/server.h"
#include "../sockets/mock_socket.h"
#include "../src/message.h"

using ::testing::_;
using ::testing::Return;

class ServerTest : public ::testing::Test {
protected:
    MockSocket mockSocket;
    Server *server;

    void SetUp() override {
        
        ON_CALL(mockSocket, socket(::testing::_, ::testing::_, ::testing::_))
            .WillByDefault(::testing::Return(1));
        ON_CALL(mockSocket, setsockopt(::testing::_, ::testing::_, ::testing::_, ::testing::NotNull(), ::testing::_))
            .WillByDefault(::testing::Return(0));
        ON_CALL(mockSocket, bind(::testing::_, ::testing::_, ::testing::_))
            .WillByDefault(::testing::Return(0));
        ON_CALL(mockSocket, listen(::testing::_, ::testing::_))
            .WillByDefault(::testing::Return(0)); 
        ON_CALL(mockSocket, accept(_, _, _))
            .WillByDefault(::testing::Return(0));
        ON_CALL(mockSocket, close(_))
            .WillByDefault(::testing::Return(0));
        Packet packet;
        ON_CALL(mockSocket, send(_, _, _, _))
            .WillByDefault(::testing::Return(sizeof(Packet)));
  
        server= new Server(1, [](void*){}, &mockSocket);      
    }
    void TearDown() override {
       // delete client;
    }
};

TEST_F(ServerTest, ConstructorCreatesSocket) {
    EXPECT_GT(server->getServerSocket(), 0);
}

