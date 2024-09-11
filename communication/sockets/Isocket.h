#ifndef ISOCKET_H
#define ISOCKET_H

#include <sys/socket.h>
#include "../../logger/logger.h"

class ISocket {
public:
    virtual int socket(int domain, int type, int protocol) = 0;
    virtual int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) = 0;
    virtual int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) = 0;
    virtual int listen(int sockfd, int backlog) = 0;
    virtual int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) = 0;
    virtual int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) = 0;
    virtual ssize_t send(int sockfd, const void *buf, size_t len, int flags) = 0;
    virtual ssize_t recv(int sockfd, void *buf, size_t len, int flags) = 0;
    virtual int close(int fd) = 0;
    virtual ~ISocket() = default;
};

#endif