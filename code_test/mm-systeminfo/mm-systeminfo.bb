DESCRIPTION = "Android performance core libraries prebuilt"
LICENSE = "QUALCOMM-TECHNOLOGY-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qti/files/qcom-licenses/${LICENSE};md5=400dd647645553d955b1053bbbfcd2de"

PV = "1.0"
PR = "r0"
 
PACKAGES = "${PN}"
FILES_${PN} +=  "/etc/systeminfo.cfg"


do_install() {
		DATE_COMPILE="apq8074-le-1-2_ap_standard_oem_36500_2+16_$(date +%Y-%m-%d)" 
		echo "$DATE_COMPILE" >> systeminfo.cfg         
    install -d ${D}/etc
    install -m 0644 systeminfo.cfg ${D}/etc    
}

