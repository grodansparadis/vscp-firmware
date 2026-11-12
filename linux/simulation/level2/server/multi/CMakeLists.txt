cmake_minimum_required(VERSION 3.10)

# set the project name
project(multi)

include_directories( PUBLIC .
                        ../../../../../src )

# add the executable ../../../../../src/vscp_server.c
#add_executable(multi multi-server.c)
add_executable(multi multi-server.c
                  ../../../../../src/vscp-server.c
                  ../../../../../src/vscp-link-protocol.c)
