#include "real_socket.h"

logger RealSocket::log("communication");

RealSocket::RealSocket(){}

int RealSocket::socket(int domain, int type, int protocol) 
{
    int sockFd = ::socket(domain, type, protocol);
    if (sockFd < 0)
    {
        RealSocket::log.logMessage(logger::LogLevel::ERROR, "socket creation error: " + std::string(strerror(errno)));
    }
    RealSocket::log.logMessage(logger::LogLevel::INFO, "create a client socket: " + std::to_string(sockFd) + std::string(" ") + std::string(strerror(errno)));
    return sockFd;
}

int RealSocket::setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) 
{
    int sockopt = ::setsockopt(sockfd, level, optname, optval, optlen);
    if (sockopt)
    {
        RealSocket::log.logMessage(logger::LogLevel::ERROR, "setsockopt failed: " + std::string(strerror(errno)));
        close(sockfd);
    }

    RealSocket::log.logMessage(logger::LogLevel::INFO, "create a server socket: " + std::to_string(sockfd));
    return sockopt;
}

int RealSocket::bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int bindAns = ::bind(sockfd, addr, addrlen);
    if (bindAns < 0)
    {
        RealSocket::log.logMessage(logger::LogLevel::ERROR, "Bind failed: " + std::string(strerror(errno)));
        close(sockfd);
    }

    return bindAns;
}

int RealSocket::listen(int sockfd, int backlog) 
{
    int listenAns = ::listen(sockfd, backlog);
    if (listenAns < 0)
    {
        RealSocket::log.logMessage(logger::LogLevel::ERROR, "Listen failed: " + std::string(strerror(errno)));
        close(sockfd);
    }

    RealSocket::log.logMessage(logger::LogLevel::INFO, "server running on port " + std::to_string(8080));
    return listenAns;
}

int RealSocket::accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int newSocket = ::accept(sockfd, addr, addrlen);
    if (newSocket < 0)
    {
        RealSocket::log.logMessage(logger::LogLevel::ERROR, "Accept failed: " + std::string(strerror(errno)));
    }

    RealSocket::log.logMessage(logger::LogLevel::INFO, "connection succeed to client socket number: " + std::to_string(sockfd));
    return newSocket;
}

int RealSocket::connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int connectAns = ::connect(sockfd, addr, addrlen);
    if (connectAns < 0)
    {
        RealSocket::log.logMessage(logger::LogLevel::ERROR, "process", "server", "Connection Failed: " + std::string(strerror(errno)));
    }

    RealSocket::log.logMessage(logger::LogLevel::INFO, "process", "server", "connection succeed: " + std::string(strerror(errno)));
    return connectAns;
}

ssize_t RealSocket::recv(int sockfd, void *buf, size_t len, int flags)
{
    int valread = ::recv(sockfd, buf, len, flags);
    const Packet *p = static_cast<const Packet *>(buf);

    if (valread < 0)
        RealSocket::log.logMessage(logger::LogLevel::ERROR, std::to_string(p->header.SrcID), std::to_string(p->header.DestID), std::string(" Error occurred: in socket ") + std::to_string(sockfd) + std::string(" ") + std::string(strerror(errno)));
    else if (valread == 0)
        RealSocket::log.logMessage(logger::LogLevel::INFO, std::to_string(p->header.SrcID), std::to_string(p->header.DestID), std::string(" connection closed: in socket ") + std::to_string(sockfd) + std::string(" ") + std::string(strerror(errno)));
    else {
        if (!p->header.DLC)
            RealSocket::log.logMessage(logger::LogLevel::INFO, std::to_string(p->header.SrcID), std::to_string(p->header.DestID), std::string("received packet number: ") + std::to_string(p->header.PSN) + std::string(", of messageId: ") + std::to_string(p->header.ID) + std::string(" ") + std::string(strerror(errno)) + " ID for connection: " + std::to_string(p->header.SrcID));
        else
            RealSocket::log.logMessage(logger::LogLevel::INFO, std::to_string(p->header.SrcID), std::to_string(p->header.DestID), std::string("received packet number: ") + std::to_string(p->header.PSN) + std::string(", of messageId: ") + std::to_string(p->header.ID) + std::string(" ") + std::string(strerror(errno)) + " Data: " + p->pointerToHex(p->data,p->header.DLC));
    }
        

    return valread;
}

ssize_t RealSocket::send(int sockfd, const void *buf, size_t len, int flags)
{
    int sendAns = ::send(sockfd, buf, len, flags);
    const Packet *p = static_cast<const Packet *>(buf);
    if (sendAns <= 0)
    {
        RealSocket::log.logMessage(logger::LogLevel::ERROR, std::to_string(p->header.SrcID), std::to_string(p->header.DestID), "sending packet number: " + std::to_string(p->header.PSN) + ", of messageId: " + std::to_string(p->header.ID) + std::string(" ") + std::string(strerror(errno)));
    }
    if (!p->header.DLC)
        RealSocket::log.logMessage(logger::LogLevel::INFO, std::to_string(p->header.SrcID), std::to_string(p->header.DestID), "sending packet number: " + std::to_string(p->header.PSN) + ", of messageId: " + std::to_string(p->header.ID) + std::string(" ") + std::string(strerror(errno)) + " ID for connection: " + std::to_string(p->header.SrcID));
    else
        RealSocket::log.logMessage(logger::LogLevel::INFO, std::to_string(p->header.SrcID), std::to_string(p->header.DestID), "sending packet number: " + std::to_string(p->header.PSN) + ", of messageId: " + std::to_string(p->header.ID) + std::string(" ") + std::string(strerror(errno)) + " Data: " + p->pointerToHex(p->data,p->header.DLC));
    return sendAns;
}

int RealSocket::close(int fd)
{
    RealSocket::log.logMessage(logger::LogLevel::INFO, "close socket number: " + std::to_string(fd));
    RealSocket::log.cleanUp();
    shutdown(fd, SHUT_RDWR);
    return ::close(fd);
}