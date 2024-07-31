#ifndef REALSOCKET_H
#define REALSOCKET_H

#include "ISocket.h"
#include <unistd.h>
#include <string.h>

class RealSocket : public ISocket
{
public:
    int socket(int domain, int type, int protocol) override
    {
        int sockFd = ::socket(domain, type, protocol);
        if (sockFd < 0)
        {
            communication::logMessage("Server", "Client", "[ERROR] Socket creation error: " + std::string(strerror(errno)));
        }

        return sockFd;
    }

    int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) override
    {
        int sockopt = ::setsockopt(sockfd, level, optname, optval, optlen);
        if (sockopt)
        {
            communication::logMessage("Server", "Client", "[ERROR] setsockopt failed: " + std::string(strerror(errno)));
            close(sockfd);
        }

        return sockopt;
    }

    int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) override
    {
        int bindAns = ::bind(sockfd, addr, addrlen);
        if (bindAns < 0)
        {
            communication::logMessage("Server", "Client", "[ERROR] Bind failed: " + std::string(strerror(errno)));
            close(sockfd);
        }

        return bindAns;
    }

    int listen(int sockfd, int backlog) override
    {
        int listenAns = ::listen(sockfd, backlog);
        if (listenAns < 0)
        {
            communication::logMessage("Server", "Client", "[ERROR] Listen failed: " + std::string(strerror(errno)));
            close(sockfd);
        }

        return listenAns;
    }

    int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) override
    {
        int newSocket = ::accept(sockfd, addr, addrlen);
        if (newSocket < 0)
        {
            communication::logMessage("Server", "Client", "[ERROR] Accept failed: " + std::string(strerror(errno)));
        }

        return newSocket;
    }

    int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) override
    {
        int connectAns = ::connect(sockfd, addr, addrlen);
        if (connectAns < 0)
        {
            communication::logMessage("Client", "Server", "[ERROR] Connection Failed: " + std::string(strerror(errno)));
        }

        return connectAns;
    }

    ssize_t recv(int sockfd, void *buf, size_t len, int flags) override
    {
        int valread = ::recv(sockfd, buf, len, flags);
        if (valread <= 0)
        {
            communication::logMessage("Server", "Client", "[ERROR] Connection closed or error occurred: " + std::string(strerror(errno)));
        }

        return valread;
    }

    ssize_t send(int sockfd, const void *buf, size_t len, int flags) override
    {
        int sendAns = ::send(sockfd, buf, len, flags);
        if(sendAns < 0){
           communication::logMessage("Server", "Client", "[ERROR] Failed to send acknowledgment: " + std::string(strerror(errno)));
        }
        
        return sendAns;
    }
    
    int close(int fd) override
    {
        return ::close(fd);
    }
};

#endif // REALSOCKET_H