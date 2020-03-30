cmake_minimum_required(VERSION 2.8)
project(rest_sever)
set(CMAKE_MACOSX_RPATH 0)

find_package(Threads REQUIRED)
# target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT})

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

#libmongoose 编译的库文件
file(GLOB mongoose_headers mongoose/*.h)
list(APPEND libcommon_srcs ${mongoose_headers})
list(APPEND libmongoose_srcs mongoose/mongoose.c)
add_library(mongoose STATIC ${libmongoose_srcs})


#libnmtool 编译的库文件
#file(GLOB nmtool_headers nmtool/*.h)
#list(APPEND libcommon_srcs ${nmtool_headers})
#list(APPEND libnmtool_srcs nmtool/nmtool.c nmtool/nmset.c)
#add_library(nmtool STATIC ${libnmtool_srcs})

#restserver
list(APPEND rest_server_srcs server/server.c)
include_directories(avs_auth)
include_directories(network)
include_directories(ssdp)
include_directories(mongoose)	#头文件
include_directories(external)
add_executable(rest_server ${rest_server_srcs})
target_link_libraries(rest_server avs_auth network ssdp mongoose   Threads::Threads)
install(TARGETS rest_server DESTINATION /usr/bin)
install(PROGRAMS rest-server DESTINATION /etc/init.d)

#restclient
set(server_src server/client.c)
add_executable(client ${server_src})
install(TARGETS client DESTINATION /usr/bin)
