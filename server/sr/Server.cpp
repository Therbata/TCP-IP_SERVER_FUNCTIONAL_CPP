//
// Created by Jakub Kramek on 21/04/2025.
//

#include "../inc//Server.h"
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <cstring>
namespace TCPServer{
    Server::Server(uint16 port) :port_(port), server_fd_(-1){
        std::memset(&addr_, 0, sizeof(addr_));
        addr_.sin_family = AF_INET;
        addr_.sin_addr.s_addr = INADDR_ANY;
        addr_.sin_port = htons(port_);
    }
    Server::~Server() {
        if (server_fd_ >= 0) {
            close(server_fd_);
        }
    }

    bool Server::start() {
        server_fd_= socket(AF_INET, SOCK_STREAM, 0);
        if(server_fd_<0){
            std::cerr<<"Error: socket() failded"<<std::endl;
            return false;
        }
        if (bind(server_fd_, reinterpret_cast<sockaddr*>(&addr_), sizeof(addr_)) < 0) {
            std::cerr << "Error: bind() failed\n";
            close(server_fd_);
            return false;
        }

        if (listen(server_fd_, 5) < 0) {
            std::cerr << "Error: listen() failed\n";
            close(server_fd_);
            return false;
        }

        std::cout << "Server listening on port " << port_ << std::endl;
        return true;
    }
    int Server::acceptClient() {
        std::cout << "Waiting for client..." << std::endl;
        int client_fd = accept(server_fd_, nullptr, nullptr);
        if (client_fd < 0) {
            std::cerr << "Error: accept() failed\n";
            return -1;
        }
        std::cout << "Client connected, fd=" << client_fd << std::endl;
        return client_fd;
    }
}//namespace TCPServer