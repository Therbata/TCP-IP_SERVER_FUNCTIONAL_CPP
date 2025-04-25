# tests/CMakeLists.txt

# Wskaż plik ze wszystkimi testami
add_executable(tests_all
        test_file.cpp
)
add_compile_definitions(TEST_RESOURCES_DIR="${CMAKE_SOURCE_DIR}/tests/resources")
target_link_libraries(tests_all
        PRIVATE
        gtest_main
        fileparser_lib
        pngparser_lib
        jpegparser_lib
)

include(GoogleTest)
gtest_discover_tests(tests_all
        PROPERTIES
        LABELS parsers
)
