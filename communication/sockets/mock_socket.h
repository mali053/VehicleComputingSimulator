
#ifndef MOCKSOCKET_H
#define MOCKSOCKET_H

#include "gmock/gmock.h"
#include "Isocket.h"

class MockSocket : public ISocket {
public:
    MOCK_METHOD(int, socket, (int domain, int type, int protocol), (override));
    MOCK_METHOD(int, setsockopt, (int sockfd, int level, int optname, const void *optval, socklen_t optlen), (override));
    MOCK_METHOD(int, bind, (int sockfd, const struct sockaddr *addr, socklen_t addrlen), (override));
    MOCK_METHOD(int, listen, (int sockfd, int backlog), (override));
    MOCK_METHOD(int, accept, (int sockfd, struct sockaddr *addr, socklen_t *addrlen), (override));
    MOCK_METHOD(int, connect, (int sockfd, const struct sockaddr *addr, socklen_t addrlen), (override));
    MOCK_METHOD(ssize_t, send, (int sockfd, const void *buf, size_t len, int flags), (override));
    MOCK_METHOD(ssize_t, recv, (int sockfd, void *buf, size_t len, int flags), (override));
    MOCK_METHOD(int, close, (int fd), (override));
};

#endif