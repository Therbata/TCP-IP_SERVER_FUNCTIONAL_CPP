//
// Created by Jakub Kramek on 21/04/2025.
//

#ifndef EDITIMAGESERVER_FILEPARSER_H
#define EDITIMAGESERVER_FILEPARSER_H
#include <vector>
#include <cstdint>
#include <fstream>
using uint8=uint8_t;
namespace ImageParser {
    class FileParser {
    public:
        virtual ~FileParser()=default;

        virtual bool check_header(const std::vector<uint8> &data) = 0;

        virtual void read(const std::string &in_file) = 0;

        virtual void write(const std::string &out_file) = 0;

    protected:
        std::vector<uint8> read_file(const std::string &file_name) {
            std::ifstream file(file_name, std::ios::binary);
            if (!file) throw std::runtime_error("Failed to open file.");

            file.seekg(0, std::ios::end);
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            std::vector<uint8> buffer(size);
            if (!file.read(reinterpret_cast<char *>(buffer.data()), size))
                throw std::runtime_error("Failed to read file.");

            return buffer;
        }
    };
} // namespace ImageParser
#endif //EDITIMAGESERVER_FILEPARSER_H
