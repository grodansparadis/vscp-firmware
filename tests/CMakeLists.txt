cmake_minimum_required(VERSION 3.16)

# set the project name
project(test LANGUAGES CXX C)

# cJSON, fetched at configure time (it used to be the third-party/cJSON git
# submodule; same commit). Use -DFETCHCONTENT_SOURCE_DIR_CJSON=<path> for a
# local checkout.
include(FetchContent)
set(_cjson_options)
if(NOT CMAKE_VERSION VERSION_LESS "3.18")
    list(APPEND _cjson_options SOURCE_SUBDIR "__sources_only__")
endif()
if(NOT CMAKE_VERSION VERSION_LESS "3.24")
    list(APPEND _cjson_options DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
endif()
FetchContent_Declare(
    cjson
    URL https://codeload.github.com/DaveGamble/cJSON/tar.gz/b2890c8d76bbb64e710585ebc0a917196b9c67e7
    URL_HASH SHA256=c3866048d822428ab289d20842c75a39c92c26e967fed2f0706f6f15e6894082
    ${_cjson_options}
)
if(CMAKE_VERSION VERSION_LESS "3.18")
    FetchContent_GetProperties(cjson)
    if(NOT cjson_POPULATED)
        FetchContent_Populate(cjson)
    endif()
else()
    FetchContent_MakeAvailable(cjson)
endif()

include(CTest)
enable_testing()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set the module path
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

message(STATUS "Build dir: " ${PROJECT_SOURCE_DIR})

find_package(GTest QUIET)

if(NOT GTest_FOUND)
    include(FetchContent)

    message(STATUS "GTest not found. Fetching googletest.")

    FetchContent_Declare(
        googletest
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )

    if(COMMAND FetchContent_MakeAvailable)
        FetchContent_MakeAvailable(googletest)
    else()
        FetchContent_GetProperties(googletest)
        if(NOT googletest_POPULATED)
            FetchContent_Populate(googletest)
            add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
        endif()
    endif()
endif()

include(GoogleTest)

# add the executable
add_executable(vscp_firmware_tests 
    unittest.cpp
    vscp-aes-unittest.cpp
    vscp-guid-parser-unittest.cpp
    crc-unittest.cpp
    crc8-unittest.cpp
    fifo-unittest.cpp
    vscp-fifo-unittest.cpp
    vscp-bootloader-unittest.cpp
    vscp-firmware-level2-unittest.cpp
    vscp-firmware-unittest.cpp
    vscp-link-protocol-unittest.cpp
    vscp-binary-unittest.cpp
    vscp-authenticated-frame-unittest.cpp
    vscp-rs485-unittest.cpp
    vscp-ws1-unittest.cpp
    vscp-ws2-unittest.cpp
    ${PROJECT_SOURCE_DIR}/../src/vscp-firmware-helper.c
    ${PROJECT_SOURCE_DIR}/../src/vscp-firmware-helper.h
    ${PROJECT_SOURCE_DIR}/../src/vscp-guid-parser.c
    ${PROJECT_SOURCE_DIR}/../src/vscp-crc.c
    ${PROJECT_SOURCE_DIR}/../src/crc8.c
    ${PROJECT_SOURCE_DIR}/../src/fifo.c
    ${PROJECT_SOURCE_DIR}/../src/vscp-fifo.c
    ${PROJECT_SOURCE_DIR}/../src/vscp-bootloader.c
    ${PROJECT_SOURCE_DIR}/../src/vscp-firmware-level2.c
    ${PROJECT_SOURCE_DIR}/../src/vscp-firmware.c
    ${PROJECT_SOURCE_DIR}/../src/vscp-aes.c
    ${PROJECT_SOURCE_DIR}/../src/vscp-link-protocol.c
    ${PROJECT_SOURCE_DIR}/../src/vscp-binary-protocol.c
    ${PROJECT_SOURCE_DIR}/../src/vscp-ws1-protocol.c
    ${PROJECT_SOURCE_DIR}/../src/vscp-ws2-protocol.c
    ${cjson_SOURCE_DIR}/cJSON.c
    )

set_target_properties(vscp_firmware_tests PROPERTIES OUTPUT_NAME test)

target_include_directories(vscp_firmware_tests PUBLIC 
    ${PROJECT_SOURCE_DIR}/../src/
    ${cjson_SOURCE_DIR}/
    ${PROJECT_SOURCE_DIR}/
    )

target_link_libraries(vscp_firmware_tests PRIVATE m
    dl
    pthread
    GTest::gtest
    GTest::gtest_main
    )

gtest_discover_tests(vscp_firmware_tests)

#add_library(MathFunctions mysqrt.cxx)
