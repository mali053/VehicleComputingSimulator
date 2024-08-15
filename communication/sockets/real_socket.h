#ifndef REALSOCKET_H
#define REALSOCKET_H

#include "Isocket.h"
#include <unistd.h>
#include <string.h>
#include "../../logger/logger.h"
#include "../include/packet.h"

class RealSocket : public ISocket
{
private:
    logger log;

public:

    RealSocket()
    {
        log = logger("communication");
    }

    int socket(int domain, int type, int protocol) override
    {
        int sockFd = ::socket(domain, type, protocol);
        if (sockFd < 0) {
            log.logMessage(logger::LogLevel::ERROR, "src", "dest", "Socket creation error: " + std::string(strerror(errno)));
        }

        log.logMessage(logger::LogLevel::INFO, "src", "dest", "create a client socket: " + std::to_string(sockFd) + std::string(strerror(errno)));
        return sockFd;
    }

    int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) override
    {
        int sockopt = ::setsockopt(sockfd, level, optname, optval, optlen);
        if (sockopt) {
            log.logMessage(logger::LogLevel::ERROR, "src", "dest", "setsockopt failed: " + std::string(strerror(errno)));
            close(sockfd);
        }

        log.logMessage(logger::LogLevel::INFO, "src", "dest", "create a server socket: "+std::to_string(sockfd));
        return sockopt;
    }

    int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) override
    {
        int bindAns = ::bind(sockfd, addr, addrlen);
        if (bindAns < 0) {
           log.logMessage(logger::LogLevel::ERROR, "src", "dest", "Bind failed: " + std::string(strerror(errno)));
            close(sockfd);
        }

        return bindAns;
    }

    int listen(int sockfd, int backlog) override
    {
        int listenAns = ::listen(sockfd, backlog);
        if (listenAns < 0) {
            log.logMessage(logger::LogLevel::ERROR, "src", "dest", "Listen failed: " + std::string(strerror(errno)));
            close(sockfd);
        }

        log.logMessage(logger::LogLevel::INFO, "src", "dest", "server running on port " + std::to_string(8080));
        return listenAns;
    }

    int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) override
    {
        int newSocket = ::accept(sockfd, addr, addrlen);
        if (newSocket < 0) {
            log.logMessage(logger::LogLevel::ERROR, "src", "dest", "Accept failed: " + std::string(strerror(errno)));
        }

        log.logMessage(logger::LogLevel::INFO, "src", "dest", "connection succeed to client socket number: " + std::to_string(sockfd));
        return newSocket;
    }

    int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) override
    {
        int connectAns = ::connect(sockfd, addr, addrlen);
        if (connectAns < 0) {
            log.logMessage(logger::LogLevel::ERROR, "src", "dest", "Connection Failed: " + std::string(strerror(errno)));
        }
            
        log.logMessage(logger::LogLevel::INFO, "src", "dest", "connection succeed to server socket" + std::string(strerror(errno)));
        return connectAns;
    }

    ssize_t recv(int sockfd, void *buf, size_t len, int flags) override
    {
        int valread = ::recv(sockfd, buf, len, flags);
        void *data = &buf;
        Packet *p = static_cast<Packet *>(data);

        if (valread < 0)
           log.logMessage(logger::LogLevel::ERROR, "src", "dest", "Error occurred: " + std::string(strerror(errno)) + "in socket" + std::to_string(sockfd));
        else if( valread == 0)
            log.logMessage(logger::LogLevel::INFO, "src", "dest", " connection closed: " + std::string(strerror(errno)) + "in socket" + std::to_string(sockfd));
        else
            log.logMessage(logger::LogLevel::INFO, "src", "dest", "received packet" + *p->data);
        return valread;
    }

    ssize_t send(int sockfd, const void *buf, size_t len, int flags) override
    {
        int sendAns = ::send(sockfd, buf, len, flags);
        void *data = &buf;
        Packet *p = static_cast<Packet *>(data);
        if(sendAns < 0) {
            log.logMessage(logger::LogLevel::ERROR, "src", "dest", "sending packet failed: " + *p->data + std::string(strerror(errno)));
        }

        log.logMessage(logger::LogLevel::INFO, "src", "dest", "sending packet succeed: " + *p->data + std::string(strerror(errno)));
        return sendAns;
    }
    
    int close(int fd) override
    {
        log.logMessage(logger::LogLevel::INFO, "src", "dest", "close client socket number: " + std::to_string(fd));
        return ::close(fd);
    }
};

#endif