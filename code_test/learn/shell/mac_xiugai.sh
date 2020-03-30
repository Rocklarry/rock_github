#########################################################################
# File Name: mac_xiugai.sh
# Author: rock
# mail: rock@163.com
# Created Time: 2017年11月30日 星期四 10时15分41秒
#########################################################################
#!/bin/bash


QRL_RAND_MAC_ROOT="00:03:7F"
QRL_DEFAULT_SOFTMAC_LOC=/lib/firmware/ath6k/AR6004/hw3.0
QRL_WIFI_MAC_FILE=softmac.bin

createSoftmacBin() {
	local mac=$1
	local location=$2
	local name=$3

	file=${location}/${name}
	echo "[INFO] Setting WLAN MAC addr to ${mac}"

	mac="$( humanMACToNumber ${mac} )"

	if [ -d ${location} ] && [ -w ${location} ]; then
		if [ -e ${name} ]; then
			if [ -w ${name} ]; then
				echo "[WARNING] Overwriting existing ${file}"
			else
				echo "[ERROR] File ${file} exists but is not writable"
				return 1
			fi
		fi
	else
		echo "[ERROR] Can't write to location ${location}"
		return 1
	fi
	# Everything is ok, create the file
	echo ${mac} | xxd -r -p > ${file}
	echo "[INFO] Wrote file ${file}"
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
	createSoftmacBin ${randMac} ${QRL_DEFAULT_SOFTMAC_LOC} ${QRL_WIFI_MAC_FILE}
}


echo "***************************************"
doRandomMac
echo "***************************************"
