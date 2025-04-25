#include <gtest/gtest.h>
#include <vector>
#include <fstream>
#include <cstdint>
#include <string>
#include "../pngParser/inc/pngParser.h"
#include "../jpegParser/inc/jpegParser.h"

using ImageParser::Png::pngParser;
using ImageParser::Jpeg::jpegParser;

TEST(PngParserTest, CheckValidHeader_ReturnTrue){
pngParser parser;
std::vector<uint8> correct_header = {0x89, 0x50, 0x4E, 0x47,
                                       0x0D, 0x0A, 0x1A, 0x0A};
EXPECT_TRUE(parser.check_header(correct_header));
}
TEST(PngParserTest, CheckInValidHeader_ReturnFalse) {
    pngParser parser;
    std::vector <uint8> wrong_header = {0x89, 0x50, 0x4E, 0x47,
                                            0x0D, 0x0A, 0x1A, 0x0A};
    wrong_header[0]=0x00;
EXPECT_FALSE(parser.check_header(wrong_header));
}
TEST(PngParserTest, CheckToShortHeader_ReturnFalse){
    pngParser parser;
    std::vector<uint8> short_header={0x89, 0x50};
    EXPECT_FALSE(parser.check_header(short_header));
}

