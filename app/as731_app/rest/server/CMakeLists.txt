cmake_minimum_required(VERSION 2.8)
project(rest_sever)
set(CMAKE_MACOSX_RPATH 0)

list(APPEND CMAKE_MODULE_PATH cmake)

#libavs_auth
file(GLOB avs_auth_headers avs_auth/*.h)
list(APPEND libcommon_srcs ${avs_auth_headers})
list(APPEND libavs_auth_srcs avs_auth/avs_auth.c)
add_library(avs_auth STATIC ${libavs_auth_srcs})

#libnetwork
file(GLOB network_headers network/*.h)
include_directories(external)
list(APPEND libcommon_srcs ${network_headers})
list(APPEND libnetwork_srcs network/network.c)
add_library(network STATIC ${libnetwork_srcs})

#libssdp
file(GLOB ssdp_headers ssdp/*.h)
list(APPEND libcommon_srcs ${ssdp_headers})
list(APPEND libssdp_srcs ssdp/ssdp.c)
add_library(ssdp STATIC ${libssdp_srcs})

#restserver
list(APPEND rest_server_srcs server/server.c)
include_directories(avs_auth)
include_directories(network)
include_directories(ssdp)
include_directories(external)
add_executable(rest_server ${rest_server_srcs})
target_link_libraries(rest_server avs_auth network ssdp mongoose nmtool)
install(TARGETS rest_server DESTINATION /usr/bin)
install(PROGRAMS rest-server DESTINATION /etc/init.d)
