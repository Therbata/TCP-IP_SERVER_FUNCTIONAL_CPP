// pngParser/sr/pngParser.cpp
#include "../inc/pngParser.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include <zlib.h>

namespace ImageParser {
    namespace Png {

        bool pngParser::check_header(const std::vector<uint8_t>& file_data) {
            if (file_data.size() < 8) return false;
            for (size_t i = 0; i < 8; ++i) {
                if (file_data[i] != PNG_HEADER[i]) {
                    return false;
                }
            }
            return true;
        }

        void pngParser::read(const std::string& in_file) {
            data_ = read_file(in_file.c_str());
            if (!check_header(data_)) throw std::runtime_error("To nie jest PNG!");
            parse_chunk();
            grayscale();
        }

        void pngParser::write(const std::string& out_file) {
            std::ofstream os(out_file, std::ios::binary);
            if (!os) throw std::runtime_error("Nie mogę otworzyć " + out_file);
            // nagłówek
            static const std::array<uint8_t,8> PNG_SIG = {0x89,0x50,0x4E,0x47,0x0D,0x0A,0x1A,0x0A};
            os.write(reinterpret_cast<const char*>(PNG_SIG.data()), PNG_SIG.size());

            for (auto& c : chunks_) {
                write_uint32be(os, c.data_length);
                os.write(reinterpret_cast<const char*>(c.type.data()), 4);
                if (c.data_length)
                    os.write(reinterpret_cast<const char*>(c.data.data()), c.data_length);
                write_uint32be(os, c.crc32);
            }
        }

        void pngParser::parse_chunk() {
            size_t offset = 8;
            while (offset + 8 < data_.size()) {
                PngChunk c;
                c.data_length = get_uint32be(data_, offset);  offset += 4;
                for (int i=0; i<4; ++i) c.type[i] = data_[offset++];
                c.data.resize(c.data_length);
                std::memcpy(c.data.data(), data_.data()+offset, c.data_length);
                offset += c.data_length;
                c.crc32 = get_uint32be(data_, offset);  offset += 4;
                chunks_.push_back(std::move(c));
                if (std::string((char*)chunks_.back().type.data(),4) == "IEND") break;
            }
        }

        uint32_t pngParser::get_uint32be(const std::vector<uint8_t>& mem, size_t off) {
            return (mem[off]<<24)|(mem[off+1]<<16)|(mem[off+2]<<8)|mem[off+3];
        }
        void pngParser::write_uint32be(std::ofstream& os, uint32_t v) {
            uint8_t buf[4] = {uint8_t(v>>24), uint8_t(v>>16), uint8_t(v>>8), uint8_t(v)};
            os.write(reinterpret_cast<char*>(buf),4);
        }

        void pngParser::grayscale() {
            auto& ihdr = chunks_[0];
            width  = get_uint32be(ihdr.data, 0);
            height = get_uint32be(ihdr.data, 4);
            bitDepth        = ihdr.data[8];
            colorType       = ihdr.data[9];
            compressionMethod = ihdr.data[10];
            filterMethod      = ihdr.data[11];
            interlaceMethod   = ihdr.data[12];
            if (bitDepth!=8 || !(colorType==2||colorType==6) || interlaceMethod!=0)
                throw std::runtime_error("Tylko 8-bit RGB/RGBA bez przeplotu.");

            std::vector<uint8_t> comp;
            for (auto& c: chunks_) if (std::string((char*)c.type.data(),4)=="IDAT")
                    comp.insert(comp.end(), c.data.begin(), c.data.end());
            std::vector<uint8_t> raw;
            decompress_idat(raw);

            unfilter_scanlines(raw);

            size_t pxSize = (colorType==6?4:3);
            for (size_t y=0; y<height; ++y) {
                size_t row = y * width * pxSize;
                for (size_t x=0; x<width; ++x) {
                    uint8_t* p = &raw[row + x*pxSize];
                    uint8_t gray = uint8_t(0.299*p[0] + 0.587*p[1] + 0.114*p[2]);
                    p[0]=p[1]=p[2]=gray;
                }
            }

            std::vector<uint8_t> to_comp;
            filter_scanlines(raw, to_comp);
            std::vector<uint8_t> newComp;
            compress_idat(to_comp, newComp);

            std::vector<PngChunk> out;
            for (auto& c: chunks_) {
                std::string t((char*)c.type.data(),4);
                if (t!="IDAT") out.push_back(c);
                if (t=="IHDR") {
                    PngChunk id;
                    id.data_length = newComp.size();
                    id.type = { 'I','D','A','T' };
                    id.data = newComp;
                    id.crc32 = crc32(0, newComp.data(), newComp.size());
                    out.push_back(std::move(id));
                }
            }
            chunks_.swap(out);
        }


       void pngParser::decompress_idat(std::vector<uint8_t>& out) {
           std::vector<uint8_t> comp;
           for (auto& c : chunks_) {
               std::string t(reinterpret_cast<char*>(c.type.data()), 4);
               if (t == "IDAT") {
                   comp.insert(comp.end(), c.data.begin(), c.data.end());
               }
           }

           size_t pxSize    = (colorType == 6 ? 4 : 3);
           size_t stride    = width * pxSize + 1;
           out.resize(stride * height);

           z_stream strm{};
           strm.zalloc   = Z_NULL;
           strm.zfree    = Z_NULL;
           strm.opaque   = Z_NULL;
           strm.next_in  = comp.data();
           strm.avail_in = comp.size();
           strm.next_out = out.data();
           strm.avail_out= out.size();

           if (inflateInit(&strm) != Z_OK)
               throw std::runtime_error("inflateInit error");
           int ret = inflate(&strm, Z_FINISH);
           if (ret != Z_STREAM_END) {
               inflateEnd(&strm);
               throw std::runtime_error("inflate error");
           }
           inflateEnd(&strm);
       }
        void pngParser::compress_idat(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
            z_stream strm{};
            deflateInit(&strm, Z_BEST_COMPRESSION);
            strm.next_in  = const_cast<Bytef*>(in.data());
            strm.avail_in = in.size();
            out.resize(in.size()+12);
            strm.next_out = out.data();
            strm.avail_out = out.size();
            int ret = deflate(&strm, Z_FINISH);
            if (ret!=Z_STREAM_END) throw std::runtime_error("deflate error");
            out.resize(strm.total_out);
            deflateEnd(&strm);
        }

        void pngParser::unfilter_scanlines(std::vector<uint8_t>& raw) {
        }

        void pngParser::filter_scanlines(const std::vector<uint8_t>& raw, std::vector<uint8_t>& out) {
            size_t pxSize = (colorType==6?4:3);
            size_t stride = width * pxSize;
            out.clear();
            for (size_t y=0; y<height; ++y) {
                out.push_back(0);
                out.insert(out.end(),
                           raw.begin()+y*stride,
                           raw.begin()+y*stride+stride);
            }
        }

    } // namespace Png
} // namespace ImageParser
