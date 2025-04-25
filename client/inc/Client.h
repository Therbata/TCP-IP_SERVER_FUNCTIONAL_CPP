//
// Created by Jakub Kramek on 21/04/2025.
//

#ifndef EDITIMAGESERVER_CLIENT_H
#define EDITIMAGESERVER_CLIENT_H
#include <cstdint>
#include <netinet/in.h>
#include <string>
using uint16=uint16_t;
namespace TCPClient {
    class Client {
    public:
        Client(const std::string& server_ip, uint16 port);
        ~Client();
        bool connectToServer();
        int getSocketFd();
    private:
        std::string server_ip_;
        uint16 port_;
        int sock_fd_;
        sockaddr_in server_addr_;
    };
}//namespace TCPClient

#endif //EDITIMAGESERVER_CLIENT_H
