cmake_minimum_required (VERSION 2.8)
# 项目信息
project (tinyalsa)
# 指定生成目标

set(CMAKE_C_COMPILER gcc)
#ADD_SUBDIRECTORY(lib)
link_libraries(
	include
	)

set(tinyalsa_src
	pcm.c 
	mixer.c
		)

	
add_library(tinyalsa SHARED ${tinyalsa_src}	)



#add_executable(tinyalsa tinyplay.c)
#add_executable(tinycap tinycap.c)
#add_executable(tinymix tinymix.c)
#add_executable(tinyhostless tinyhostless.c)
#add_executable(tinypcminfo tinypcminfo.c)
