cmake_minimum_required(VERSION 3.20)
project(EditImageServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Znajdź bibliotekę ZLIB (potrzebna do kompresji/dekompresji PNG)
find_package(ZLIB REQUIRED)

# ————————————————
# 1) FileParser = header-only (INTERFACE)
add_library(fileparser_lib INTERFACE)
target_include_directories(fileparser_lib INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/FileParser/inc
)

# ————————————————
# 2) PNG parser
add_library(pngparser_lib
        pngParser/inc/pngParser.h
        pngParser/sr/pngParser.cpp
)
target_include_directories(pngparser_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/pngParser/inc
)
target_link_libraries(pngparser_lib
        PRIVATE fileparser_lib
        PUBLIC  ZLIB::ZLIB
)

# ————————————————
# 3) JPEG parser
add_library(jpegparser_lib
        jpegParser/inc/jpegParser.h
        jpegParser/sr/jpegParser.cpp
)
target_include_directories(jpegparser_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/jpegParser/inc
)
target_link_libraries(jpegparser_lib
        PRIVATE fileparser_lib
)

# ————————————————
# 4) Server
add_library(server_lib
        server/inc/Server.h
        server/sr/Server.cpp
)
target_include_directories(server_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/server/inc
)
target_link_libraries(server_lib
        PRIVATE pngparser_lib
        PRIVATE jpegparser_lib
        PRIVATE fileparser_lib
)

# ————————————————
# 5) Główna aplikacja (opcjonalnie)
add_executable(EditImageServer main.cpp)
target_link_libraries(EditImageServer
        PRIVATE server_lib
)

# ————————————————
# 6) GoogleTest (FetchContent)
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/release-1.12.1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ————————————————
# 7) Testy
enable_testing()
add_subdirectory(tests)
