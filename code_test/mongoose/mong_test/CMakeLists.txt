cmake_minimum_required(VERSION 2.8)
project(rest_sever)
set(CMAKE_MACOSX_RPATH 0)

find_package(Threads REQUIRED)
# target_link_libraries(main ${CMAKE_THREAD_LIBS_INIT})

list(APPEND CMAKE_MODULE_PATH cmake)



#libmongoose 编译的库文件
file(GLOB mongoose_headers mongoose/*.h)
list(APPEND libcommon_srcs ${mongoose_headers})
list(APPEND libmongoose_srcs mongoose/mongoose.c)
add_library(mongoose STATIC ${libmongoose_srcs})




#restserver
list(APPEND mong_test_srcs mong_test.c)

include_directories(mongoose)	#头文件

add_executable(mong_test ${mong_test_srcs})
target_link_libraries(mong_test   mongoose  )



