cmake_minimum_required(VERSION 3.11)

# set the project name
project(test LANGUAGES CXX C)

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
    ${PROJECT_SOURCE_DIR}/../common/vscp-firmware-helper.c
    ${PROJECT_SOURCE_DIR}/../common/vscp-firmware-helper.h
    ${PROJECT_SOURCE_DIR}/../common/vscp-guid-parser.c
    ${PROJECT_SOURCE_DIR}/../common/crc.c
    ${PROJECT_SOURCE_DIR}/../common/crc8.c
    ${PROJECT_SOURCE_DIR}/../common/fifo.c
    ${PROJECT_SOURCE_DIR}/../common/vscp-fifo.c
    ${PROJECT_SOURCE_DIR}/../common/vscp-bootloader.c
    ${PROJECT_SOURCE_DIR}/../common/vscp-firmware-level2.c
    ${PROJECT_SOURCE_DIR}/../common/vscp-firmware.c
    ${PROJECT_SOURCE_DIR}/../common/vscp-aes.c
    ${PROJECT_SOURCE_DIR}/../third-party/cJSON/cJSON.c
    )

set_target_properties(vscp_firmware_tests PROPERTIES OUTPUT_NAME test)

target_include_directories(vscp_firmware_tests PUBLIC 
    ${PROJECT_SOURCE_DIR}/../common/
    ${PROJECT_SOURCE_DIR}/../third-party/cJSON/
    )

target_link_libraries(vscp_firmware_tests PRIVATE m
    dl
    pthread
    GTest::gtest
    GTest::gtest_main
    )

gtest_discover_tests(vscp_firmware_tests)

#add_library(MathFunctions mysqrt.cxx)
