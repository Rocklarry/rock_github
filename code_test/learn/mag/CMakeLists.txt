
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/home/idea/code_test/mag/cmake_hexagon")
INCLUDE_DIRECTORIES(
dev_fs_lib_i2c.h
snav_drivers.h
snav_drivers_io.h
)
#mag0 hmc5883l driver 
set(mag0_hmc5883l_driver_srcs
mag0_hmc5883l_driver.cpp
snav_drivers_io.c 
) 


link_directories (/home/idea/code_test/mag/libsnav_q6.so)
add_library(mag0_hmc5883l_driver SHARED ${mag0_hmc5883l_driver_srcs}) 
set_property(TARGET mag0_hmc5883l_driver PROPERTY POSITION_INDEPENDENT_CODE TRUE) 
#target_link_libraries( mag0_hmc5883l_driver snav_q6 ) 
target_link_libraries( mag0_hmc5883l_driver ) 
