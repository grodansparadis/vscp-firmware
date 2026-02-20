cmake_minimum_required(VERSION 3.10)

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

find_package(GTest REQUIRED)
include(GoogleTest)

# add the executable
add_executable(vscp_firmware_tests 
    unittest.cpp
    ${PROJECT_SOURCE_DIR}/../common/vscp-firmware-helper.c
    ${PROJECT_SOURCE_DIR}/../common/vscp-firmware-helper.h
    ${PROJECT_SOURCE_DIR}/../common/crc.c
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
    gtest
    gtest_main
    )    

gtest_discover_tests(vscp_firmware_tests)

#add_library(MathFunctions mysqrt.cxx)
