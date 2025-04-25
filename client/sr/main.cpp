
#include "../inc/Client.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <arpa/inet.h>

static bool sendAll(int fd, const void* buf, size_t len) {
    size_t sent = 0;
    const char* ptr = reinterpret_cast<const char*>(buf);
    while (sent < len) {
        ssize_t s = send(fd, ptr + sent, len - sent, 0);
        if (s <= 0) return false;
        sent += s;
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) { std::cerr<<"Użycie: "<<argv[0]<<" <plik>\n"; return 1; }
    std::string path = argv[1];
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) return 1;

    std::vector<uint8_t> data((std::istreambuf_iterator<char>(ifs)), {});
    ifs.close();

    TCPClient::Client client("127.0.0.1", 8080);
    if (!client.connectToServer()) return 1;
    int sock = client.getSocketFd();

    uint32_t nN = htonl(path.size());
    sendAll(sock, &nN, sizeof(nN));
    sendAll(sock, path.data(), path.size());

    uint32_t sN = htonl(data.size());
    sendAll(sock, &sN, sizeof(sN));
    sendAll(sock, data.data(), data.size());

    std::cout << "Wysłano: " << path << "\n";
    return 0;
}