cmake_minimum_required(VERSION 3.10)

# set the project name
project(multi)

include_directories( PUBLIC .
                        ../../../../common )

# add the executable ../../../../common/vscp_server.c
#add_executable(multi multi-server.c)
add_executable(multi multi-server.c
                  ../../../../common/vscp-server.c
                  ../../../../common/vscp-link-protocol.c)
