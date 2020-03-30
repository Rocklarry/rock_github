#########################################################################
# File Name: mac.sh
# Author: Rock
# mail: ruidongren@163.com
# Created Time: 2017年04月10日 星期一 16时40分33秒
#########################################################################
#!/bin/bash

	
 if [ ! -f tmp/hostapd.conf.tmp ]; then
        touch /tmp/hostapd.conf.tmp
	echo ""
	echo "new tmp"
	echo " "
 fi


  new_md5=`md5sum /data/misc/wifi/hostapd.conf | cut -d' ' -f1`
  old_md5=`md5sum /var/volatile/tmp/hostapd.conf.tmp | cut -d' ' -f1`
   echo "new_md5 = ${new_md5}    old_md5= ${old_md5}"
 if [ "$old_md5" = "$new_md5" ]; then
    cp  /tmp/hostapd.conf.tmp /data/misc/wifi/hostapd.conf
 else

  rand=`od -An -N2 -tu2 /dev/urandom | sed -e 's/ //g'`
  sed -e "s/^ssid=/ssid=Follow_$rand/g" /data/misc/wifi/hostapd.conf > /tmp/hostapd.conf.tmp
  cp  /tmp/hostapd.conf.tmp /data/misc/wifi/hostapd.conf

  reboot
 fi


