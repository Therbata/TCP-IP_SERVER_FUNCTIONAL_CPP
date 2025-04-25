//
// Created by Jakub Kramek on 21/04/2025.
//

#ifndef EDITIMAGESERVER_JPEGPARSER_H
#define EDITIMAGESERVER_JPEGPARSER_H
#include <string>
#include "../../FileParser/inc/FileParser.h"
#include <vector>
#include <cstdint>
#include <fstream>
using uint8=uint8_t;
using uint16=uint16_t;
using uint32=uint32_t;
namespace ImageParser {
    namespace Jpeg {
        struct JpegSegment {
            uint16 marker;
            uint16 length;
            std::vector<uint8> data;
            std::vector<uint8> scan_data;
        };

        class jpegParser : public FileParser {
        public:
            jpegParser() = default;

            const uint16 JPEG_SOI = 0xFFD8;
            const uint16 JPEG_SOS = 0xFFDA;
            const uint16 JPEG_EOI = 0xFFD9;

            bool check_header(const std::vector<uint8> &date_file) override;

            void read(const std::string &in_file) override;

            void write(const std::string &out_file) override;

        private:
            std::vector<uint8> data_;
            std::vector<JpegSegment> segments_;

            uint16 get_uint16be(const std::vector<uint8> &mem, size_t offset);

            void write_uint16be(std::ofstream &os, uint16 v);

            JpegSegment read_segment(size_t &offset);

            std::vector<JpegSegment> parse_segments();
        };
    } // namespace Jpeg
}// namespace ImageParser

#endif //EDITIMAGESERVER_JPEGPARSER_H
