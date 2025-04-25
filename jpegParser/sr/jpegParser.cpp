//
// Created by Jakub Kramek on 21/04/2025.
//

#include "../inc/jpegParser.h"
#include <iostream>
namespace ImageParser {
    namespace Jpeg {
        bool jpegParser::check_header(const std::vector<uint8> &date_file) {
            return date_file.size() >= 2
                   && date_file[0] == 0xFF
                   && date_file[1] == 0xD8;
        }

        void jpegParser::read(const std::string &in_file) {
            data_ = read_file(in_file.c_str());
            if (!check_header(data_)) {
                throw std::runtime_error("To nie jest poprawny JPEG: " + in_file);
            }
            std::cout << "JPEG: " << in_file << " wczytany poprawnie\n";
            segments_ = parse_segments();
        }

        void jpegParser::write(const std::string &out_file) {
            std::ofstream os(out_file, std::ios::binary);
            if (!os) throw std::runtime_error("Nie mogę otworzyć " + out_file + " do zapisu");
            os.put(0xFF);
            os.put(0xD8);
            for (size_t i = 1; i < segments_.size(); ++i) {
                auto &s = segments_[i];
                os.put(0xFF);
                os.put(uint8(s.marker & 0xFF));
                if (s.marker == 0xFFDA) {
                    write_uint16be(os, s.length);
                    os.write(reinterpret_cast<const char *>(s.data.data()), s.data.size());
                    os.write(reinterpret_cast<const char *>(s.scan_data.data()), s.scan_data.size());
                } else if (s.marker >= 0xFFD0 && s.marker <= 0xFFD7) {
                } else if (s.marker == JPEG_EOI) {
                    break;
                } else {
                    write_uint16be(os, s.length);
                    os.write(reinterpret_cast<const char *>(s.data.data()), s.data.size());
                }
            }
            std::cout << "Zapisano " << out_file << " z " << segments_.size() << " segmentami\n";
        }

        uint16 jpegParser::get_uint16be(const std::vector<uint8> &mem, size_t offset) {
            uint16 result = 0;
            for (size_t i = 0; i < 2; ++i) {
                result = (result << 8) | mem[offset + i];
            }
            return result;
        }

        void jpegParser::write_uint16be(std::ofstream &os, uint16 v) {
            char b[2] = {char((v >> 8) & 0xFF), char(v & 0xFF)};
            os.write(b, 2);
        }
        JpegSegment jpegParser::read_segment(size_t &offset) {
            while (offset + 1 < data_.size() && data_[offset] != 0xFF)
                offset++;
            if (offset + 1 >= data_.size())
                throw std::runtime_error("Nieoczekiwany koniec danych");

            uint16 mk = (uint16(0xFF) << 8) | data_[offset + 1];
            offset += 2;
            JpegSegment seg{mk, 0, {}, {}};

            if (mk == JPEG_EOI) {
                std::cout << "EOI\n";
                return seg;
            }
            if (mk == JPEG_SOS) {
                seg.length = get_uint16be(data_, offset);
                offset += 2;
                uint16 hdr = seg.length - 2;
                seg.data.assign(data_.begin() + offset,
                                data_.begin() + offset + hdr);
                offset += hdr;

                while (offset + 1 < data_.size()) {
                    if (data_[offset] == 0xFF && data_[offset + 1] == 0xD9)
                        break;
                    seg.scan_data.push_back(data_[offset++]);
                }

                std::cout << "SOS, hdr=" << hdr
                          << ", scan_total=" << seg.scan_data.size()
                          << "\n";
                return seg;
            }
            seg.length = get_uint16be(data_, offset);
            offset += 2;
            uint16 dl = seg.length - 2;
            seg.data.assign(data_.begin() + offset,
                            data_.begin() + offset + dl);
            offset += dl;

            std::cout << "Marker 0x" << std::hex << mk
                      << std::dec << ", len=" << dl << "\n";
            return seg;

        }

        std::vector<JpegSegment> jpegParser::parse_segments() {
            std::vector<JpegSegment> v;
            v.push_back({JPEG_SOI, 0, {}, {}});
            size_t off = 2;
            while (off + 1 < data_.size()) {
                auto s = read_segment(off);
                v.push_back(std::move(s));
                if (v.back().marker == JPEG_EOI) break;
            }
            return v;
        }
    }//namespace Jpeg
}//namespace ImageParser