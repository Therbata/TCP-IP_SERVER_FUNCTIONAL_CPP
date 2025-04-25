//
// Created by Jakub Kramek on 21/04/2025.
//

#ifndef EDITIMAGESERVER_SERVER_H
#define EDITIMAGESERVER_SERVER_H
#include <netinet/in.h>
#include <cstdint>
using uint16=uint16_t;
namespace TCPServer {
    class Server {
    public:
        explicit Server(uint16 port);

        ~Server();

        bool start();

        int acceptClient();

    private:
        uint16 port_;
        int server_fd_;
        sockaddr_in addr_;
    };
} //namespace TCPServer
#endif //EDITIMAGESERVER_SERVER_H
