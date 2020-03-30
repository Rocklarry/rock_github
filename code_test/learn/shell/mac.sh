#########################################################################
# File Name: mac.sh
# Author: Rock
# mail: ruidongren@163.com
# Created Time: 2017年04月10日 星期一 16时40分33秒
#########################################################################
#!/bin/bash

# random number for changing the MAC address

QRL_RAND_MAC_ROOT="00:03:7F"

 mac=$(( $(od -An -N2 -i /dev/urandom)%(100) ))

randMac="${QRL_RAND_MAC_ROOT}:${mac}"
mac=$(( $(od -An -N2 -i /dev/urandom)%(100) ))


randMac="${randMac}:${mac}"
echo "[INFO] Generated random WLAN MAC address: $randMac"
