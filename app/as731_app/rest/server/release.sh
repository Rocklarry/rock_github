#!/bin/bash

echo "rest_server release"

source build/header.rc
source build/install.rc

dst_dir=${CONFIG_SYNA_SDK_REL_PATH}/application/rest_server
INSTALL_D ${topdir}/application/rest_server ${CONFIG_SYNA_SDK_REL_PATH}/application

rm ${dst_dir}/release.sh
