
INCLUDE_DIRECTORIES(
device_drivers/mag/dev_fs_lib_i2c.h
device_drivers/mag/snav_drivers.h
device_drivers/mag/snav_drivers_io.h
)
#mag0 hmc5883l driver 
set(mag0_hmc5883l_driver_srcs
device_drivers/mag/mag0_hmc5883l_driver.cpp
device_drivers/mag/snav_drivers_io.c 
) 


add_library(mag0_hmc5883l_driver SHARED ${mag0_hmc5883l_driver_srcs}) 
set_property(TARGET mag0_hmc5883l_driver PROPERTY POSITION_INDEPENDENT_CODE TRUE) 
#target_link_libraries( mag0_hmc5883l_driver snav_q6 ) 
target_link_libraries( mag0_hmc5883l_driver ) 


INCLUDE_DIRECTORIES(
device_drivers/mag5883/dev_fs_lib_i2c.h
device_drivers/mag5883/snav_drivers.h
device_drivers/mag5883/snav_drivers_io.h
)
#mag0 hmc5883 driver 
set(mag0_hmc5883_driver_srcs
device_drivers/mag5883/mag0_hmc5883_driver.cpp
device_drivers/mag5883/snav_drivers_io.c 
) 


add_library(mag0_hmc5883_driver SHARED ${mag0_hmc5883_driver_srcs}) 
set_property(TARGET mag0_hmc5883_driver PROPERTY POSITION_INDEPENDENT_CODE TRUE) 
#target_link_libraries( mag0_hmc5883_driver snav_q6 ) 
target_link_libraries( mag0_hmc5883_driver ) 
