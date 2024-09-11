#ifndef REALSOCKET_H
#define REALSOCKET_H

#include "Isocket.h"
#include <unistd.h>
#include <string.h>
#include "../include/packet.h"

class RealSocket : public ISocket
{
public:
    static logger log;

    RealSocket();

    int socket(int domain, int type, int protocol) override;

    int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) override;

    int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) override;

    int listen(int sockfd, int backlog) override;
    
    int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) override;
    
    int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) override;

    ssize_t recv(int sockfd, void *buf, size_t len, int flags) override;

    ssize_t send(int sockfd, const void *buf, size_t len, int flags) override;
    
    int close(int fd) override;
};
#endif