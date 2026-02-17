cmake_minimum_required(VERSION 3.10)

# set the project name
project(test LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set the module path
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

message(STATUS "Build dir: " ${PROJECT_SOURCE_DIR})

find_package(GTest REQUIRED)

# add the executable
add_executable(test 
    unittest.cpp
    ${PROJECT_SOURCE_DIR}/../common/vscp-firmware-helper.c
    ${PROJECT_SOURCE_DIR}/../common/vscp-firmware-helper.h
    ${PROJECT_SOURCE_DIR}/../common/crc.c
    ${PROJECT_SOURCE_DIR}/../common/vscp-aes.c
    ${PROJECT_SOURCE_DIR}/../third-party/cJSON/cJSON.c
    )

target_include_directories(test PUBLIC 
    ${PROJECT_SOURCE_DIR}/../common/
    ${PROJECT_SOURCE_DIR}/../third-party/cJSON/
    )

target_link_libraries(test PRIVATE m
    dl
    pthread
    gtest
    gtest_main
    )    

#add_library(MathFunctions mysqrt.cxx)
