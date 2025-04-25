//
// Created by Jakub Kramek on 21/04/2025.
//
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include "../inc/Client.h"
namespace TCPClient{
    Client::Client(const std::string &server_ip, uint16 port) : server_ip_(server_ip), port_(port), sock_fd_(-1){
        std::memset(&server_addr_, 0, sizeof(server_addr_));
        server_addr_.sin_family = AF_INET;
        server_addr_.sin_port = htons(port_);
        inet_pton(AF_INET, server_ip_.c_str(), &server_addr_.sin_addr);
    }
    Client::~Client() {
        if (sock_fd_ >= 0) {
            close(sock_fd_);
        }
    }

    bool Client::connectToServer() {
        sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (sock_fd_ < 0) {
            std::cerr << "Error: socket() failed\n";
            return false;
        }

        std::cout << "Connecting to " << server_ip_ << ":" << port_ << "...\n";
        if (connect(sock_fd_, reinterpret_cast<sockaddr*>(&server_addr_), sizeof(server_addr_)) < 0) {
            std::cerr << "Error: connect() failed\n";
            close(sock_fd_);
            return false;
        }

        std::cout << "Connected to server\n";
        return true;
    }

    int Client::getSocketFd() {
        return sock_fd_;
    }
}//namespace TCPClient
