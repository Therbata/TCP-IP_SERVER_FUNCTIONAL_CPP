# TCP-IP_SERVER_FUNCTIONAL_CPP
# EditImageServer

A C++ project for parsing and editing PNG images — including grayscale conversion, chunk analysis, reading/writing PNG files, and more.  
Runs as a TCP/IP server

## Technologies

- C++
- CMake
- Google Test (unit testing)
- Zlib (for PNG compression support)

## Compile in terminal
```bash
# Build server
server:
 g++ -std=c++17 \       
  -IjpegParser/inc \
  -IpngParser/inc \
  -IFileParser/inc \
  -IServer/inc \
  main.cpp \
  jpegParser/sr/jpegParser.cpp \
  pngParser/sr/pngParser.cpp \
  Server/sr/Server.cpp \
  -o serverAPP \
  -lpthread -lz

# Build client
Client:
g++ -std=c++17 -IClient/inc -IFileParser/inc -IpngParser/inc -IjpegParser/inc Client/sr/Client.cpp Client/sr/main.cpp -o clientAPP -lpthread -lz 

#run server
./serverAPP

# Run client (with image file path as argument)
./clientAPP path/to/image.png
```
## Build & Run unit tests

```bash
# Configure and build the project
cmake -S . -B build
cmake --build build

# Run unit tests
ctest --test-dir build -R PngParserTest

