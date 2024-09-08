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
            log.logMessage(logger::LogLevel::ERROR, "socket creation error: " + std::string(strerror(errno)));
        }
        log.logMessage(logger::LogLevel::INFO, "create a client socket: " + std::to_string(sockFd) + std::string(" ") + std::string(strerror(errno)));
        return sockFd;
    }

    int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) override
    {
        int sockopt = ::setsockopt(sockfd, level, optname, optval, optlen);
        if (sockopt) {
            log.logMessage(logger::LogLevel::ERROR, "setsockopt failed: " + std::string(strerror(errno)));
            close(sockfd);
        }

        log.logMessage(logger::LogLevel::INFO, "create a server socket: " + std::to_string(sockfd));
        return sockopt;
    }

    int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) override
    {
        int bindAns = ::bind(sockfd, addr, addrlen);
        if (bindAns < 0) {
           log.logMessage(logger::LogLevel::ERROR, "Bind failed: " + std::string(strerror(errno)));
            close(sockfd);
        }

        return bindAns;
    }

    int listen(int sockfd, int backlog) override
    {
        int listenAns = ::listen(sockfd, backlog);
        if (listenAns < 0) {
            log.logMessage(logger::LogLevel::ERROR, "Listen failed: " + std::string(strerror(errno)));
            close(sockfd);
        }

        log.logMessage(logger::LogLevel::INFO, "server running on port " + std::to_string(8080));
        return listenAns;
    }

    int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) override
    {
        int newSocket = ::accept(sockfd, addr, addrlen);
        if (newSocket < 0) {
            log.logMessage(logger::LogLevel::ERROR, "Accept failed: " + std::string(strerror(errno)));
        }

        log.logMessage(logger::LogLevel::INFO, "connection succeed to client socket number: " + std::to_string(sockfd));
        return newSocket;
    }

    int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) override
    {
        int connectAns = ::connect(sockfd, addr, addrlen);
        if (connectAns < 0) {
            log.logMessage(logger::LogLevel::ERROR, "process", "server", "Connection Failed: " + std::string(strerror(errno)));
        }
            
        log.logMessage(logger::LogLevel::INFO, "process", "server", "connection succeed: " + std::string(strerror(errno)));
        return connectAns;
    }

    ssize_t recv(int sockfd, void *buf, size_t len, int flags) override
    {
        int valread = ::recv(sockfd, buf, len, flags);
        const Packet *p = static_cast<const Packet *>(buf);

        if (valread < 0)
           log.logMessage(logger::LogLevel::ERROR, std::to_string(p->header.SrcID), std::to_string(p->header.DestID),  std::string("Error occurred: in socket ") + std::to_string(sockfd) + std::string(" ") + std::string(strerror(errno)));
        else if( valread == 0)
            log.logMessage(logger::LogLevel::INFO, std::to_string(p->header.SrcID), std::to_string(p->header.DestID), std::string(" connection closed: in socket") + std::to_string(sockfd) + std::string(" ") + std::string(strerror(errno)));
        else
            log.logMessage(logger::LogLevel::INFO, std::to_string(p->header.SrcID), std::to_string(p->header.DestID),  std::string("received packet number: ") + std::to_string(p->header.PSN) +  std::string(", of messageId: ") + std::to_string(p->header.ID) + std::string(" ") + std::string(strerror(errno)));
        return valread;
    }

    ssize_t send(int sockfd, const void *buf, size_t len, int flags) override
    {
        int sendAns = ::send(sockfd, buf, len, flags);
        const Packet *p = static_cast<const Packet *>(buf);
        if(sendAns < 0) {
            log.logMessage(logger::LogLevel::ERROR, std::to_string(p->header.SrcID), std::to_string(p->header.DestID), "sending packet number: " + std::to_string(p->header.PSN) + ", of messageId: " + std::to_string(p->header.ID) + std::string(" ") + std::string(strerror(errno)));
        }

        log.logMessage(logger::LogLevel::INFO, std::to_string(p->header.SrcID), std::to_string(p->header.DestID), "sending packet number: " + std::to_string(p->header.PSN) + ", of messageId: " + std::to_string(p->header.ID) + std::string(" ") + std::string(strerror(errno)));
        return sendAns;
    }
    
    int close(int fd) override
    {
        log.logMessage(logger::LogLevel::INFO, "close socket number: " + std::to_string(fd));
        log.cleanUp();
        shutdown(fd, SHUT_RDWR);
        return ::close(fd);
    }
};

#endif