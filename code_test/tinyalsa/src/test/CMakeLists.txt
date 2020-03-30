cmake_minimum_required (VERSION 2.8)
# 项目信息
project (tinyalsa)
# 指定生成目标

link_libraries(
	./include
	)

add_library(tinyalsa  
	mixer.c  
	pcm.c
	)


#add_executable(tinyplay tinyplay.c)
#add_executable(tinycap tinycap.c)
#add_executable(tinymix tinymix.c)
#add_executable(tinyhostless tinyhostless.c)
#add_executable(tinypcminfo tinypcminfo.c)
