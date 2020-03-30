DESCRIPTION = "Android performance core libraries prebuilt"
LICENSE = "QUALCOMM-TECHNOLOGY-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qti/files/qcom-licenses/${LICENSE};md5=400dd647645553d955b1053bbbfcd2de"

PV = "1.0.0"
PR = "r0"
 
PACKAGES = "${PN}"
SRC_URI += "file://wifi_check_reset"


do_install() {       
    install -d ${D}/usr/bin
    install -m 0777 ${WORKDIR}/wifi_check_reset ${D}/usr/bin

    
}

