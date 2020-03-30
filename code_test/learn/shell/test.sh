#!/bin/bash

for entry in $tmp ; do
	ttydev=`echo "$entry" | sed -e 's/^[0-9]*\;//' -e 's/\;.*//'`
	done

echo $ttydev


QRL_RAND_MAC_ROOT="00:03:7F"

demoFun(){
	echo " shell 函数"
}

doRandomMac() {
	# Get a 2-digit random number for changing the MAC address
	mac=$(( $(od -An -N2 -i /dev/urandom)%(100) ))
	echo $mac
	if [ $mac -lt 10 ]; then
		mac=0$mac
		echo $mac
	fi
	randMac="${QRL_RAND_MAC_ROOT}:${mac}"
	mac=$(( $(od -An -N2 -i /dev/urandom)%(100) ))
	echo $mac
	if [ $mac -lt 10 ]; then
		mac=0$mac
		echo $mac
	fi
	randMac="${randMac}:${mac}"
	mac=$(( $(od -An -N2 -i /dev/urandom)%(100) ))
	echo $mac
	if [ $mac -lt 10 ]; then
		mac=0$mac
		echo $mac
	fi
	randMac="${randMac}:${mac}"
	echo "[INFO] Generated random WLAN MAC address: $randMac"
	#createSoftmacBin ${randMac} ${QRL_DEFAULT_SOFTMAC_LOC} ${QRL_WIFI_MAC_FILE}
}

 ifconfig
echo $1

echo "***************************************"
demoFun
echo "***************************************"

doRandomMac
