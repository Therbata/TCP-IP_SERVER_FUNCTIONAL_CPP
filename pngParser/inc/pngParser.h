//
// Created by Jakub Kramek on 21/04/2025.
//

#ifndef EDITIMAGESERVER_PNGPARSER_H
#define EDITIMAGESERVER_PNGPARSER_H
#include <vector>
#include <array>
#include <cstdint>
#include <fstream>
#include <string>
#include <zlib.h>
#include "../../FileParser/inc/FileParser.h"
#include <iostream>
using uint32=uint32_t;
using uint8=uint8_t;
using uint16=uint16_t;
namespace ImageParser {
    namespace Png{
struct PngChunk{
    uint32 data_length;
    std::array<uint8,4> type;
    std::vector<uint8> data;
    uint32 crc32;
};
    class pngParser : public FileParser {
    public:
        pngParser() = default;

        const std::array<uint8, 8> PNG_HEADER = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};

        bool check_header(const std::vector<uint8> &file_data) override;

        void read(const std::string &in_file) override;

        void write(const std::string &out_file) override;

        void grayscale();

        void setChunksForTest(const std::vector<PngChunk>& chunks) {
            chunks_ = chunks;
        }
        const std::vector<PngChunk>& getChunksForTest() const {
            return chunks_;
        }
    private:
        std::vector<uint8> data_;
        std::vector<PngChunk> chunks_;
        uint32 width, height;
        uint8  bitDepth, colorType, compressionMethod, filterMethod, interlaceMethod;

        uint16 get_uint16be(const std::vector<uint8> &mem, size_t offset);

        uint32 get_uint32be(const std::vector<uint8> &mem, size_t offset);

        void write_uint32be(std::ofstream &os, uint32 v);

        void parse_chunk();

        void decompress_idat(std::vector<uint8_t>& out);
        void compress_idat(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);

        void unfilter_scanlines(std::vector<uint8_t>& raw);
        void filter_scanlines(const std::vector<uint8_t>& raw, std::vector<uint8_t>& out);
    };

}//namespace Png
}//namespace ImageParser
#endif //EDITIMAGESERVER_PNGPARSER_H
