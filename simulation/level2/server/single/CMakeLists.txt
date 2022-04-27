cmake_minimum_required(VERSION 3.10)

# set the project name
project(single)


#target_include_directories(sever PUBLIC
#                           "${PROJECT_BINARY_DIR}"
#                           )

# add the executable
add_executable(single single-server.c)
