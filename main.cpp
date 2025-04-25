// EditImageServer/main.cpp

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <filesystem>
#include <arpa/inet.h>
#include <unistd.h>

#include "server/inc/Server.h"
#include "FileParser/inc/FileParser.h"
#include "pngParser/inc/pngParser.h"
#include "jpegParser/inc/jpegParser.h"

static bool recvAll(int fd, void* buf, size_t len) {
    size_t rec = 0;
    while (rec < len) {
        ssize_t r = ::recv(fd, (char*)buf + rec, len - rec, 0);
        if (r <= 0) return false;
        rec += r;
    }
    return true;
}

int main() {
    using namespace TCPServer;
    Server server(8080);
    if (!server.start()) {
        std::cerr << "Nie udało się uruchomić serwera na porcie 8080\n";
        return 1;
    }

    std::cout << "Czekam na połączenie...\n";
    int clientFd = server.acceptClient();
    if (clientFd < 0) {
        std::cerr << "Błąd przy akceptowaniu klienta\n";
        return 1;
    }
    std::cout << "Klient połączony, odbieram plik...\n";

    uint32_t nameN;
    if (!recvAll(clientFd, &nameN, sizeof(nameN))) return 1;
    uint32_t nameLen = ntohl(nameN);

    std::vector<char> nameBuf(nameLen);
    if (!recvAll(clientFd, nameBuf.data(), nameLen)) return 1;
    std::string fname(nameBuf.begin(), nameBuf.end());

    uint32_t sizeN;
    if (!recvAll(clientFd, &sizeN, sizeof(sizeN))) return 1;
    uint32_t fileSize = ntohl(sizeN);

    std::vector<uint8_t> data(fileSize);
    if (!recvAll(clientFd, data.data(), fileSize)) return 1;
    std::cout << "Odebrano: " << fname << " (" << fileSize << " B)\n";

    {
        std::ofstream ofs(fname, std::ios::binary);
        ofs.write(reinterpret_cast<char*>(data.data()), data.size());
    }

    const std::string outDir = "images/converted";
    std::filesystem::create_directories(outDir);

    std::string ext = fname.substr(fname.find_last_of('.'));
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    std::unique_ptr<ImageParser::FileParser> parser;
    if (ext == ".png") {
        parser = std::make_unique<ImageParser::Png::pngParser>();
    }
    else if (ext == ".jpg" || ext == ".jpeg") {
        parser = std::make_unique<ImageParser::Jpeg::jpegParser>();
    }
    else {
        std::cerr << "Nieobsługiwany format: " << ext << "\n";
        close(clientFd);
        return 1;
    }

    try {
        parser->read(fname);

        std::string filename = std::filesystem::path(fname).filename().string();
        std::string outPath = outDir + "/" + filename;
        parser->write(outPath);

        std::cout << "Zapisano przekonwertowany plik: " << outPath << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Błąd podczas przetwarzania: " << e.what() << "\n";
        close(clientFd);
        return 1;
    }

    close(clientFd);
    return 0;
}
