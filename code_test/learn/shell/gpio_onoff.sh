#########################################################################
# File Name: led_gpio.sh
# Author: rock
# mail: rock@163.com
# Created Time: 2017年11月17日 星期五 10时29分10秒
#########################################################################
#!/bin/bash

PIN=$1;

if [ $2 == "on" ];  then
  echo "NO EDN_LED RED"
  echo $PIN > sys/class/gpio/export      
  echo out  > /sys/class/gpio/gpio$PIN/direction   
  echo 1 > sys/class/gpio/gpio$PIN/value
  
  elif [ $2 == "off" ];  then
  echo "OFF EDN_LED RED"
  echo 0 > sys/class/gpio/gpio$PIN/value
  echo $PIN > sys/class/gpio/unexport
  else
 echo "led_gpio error"
fi


