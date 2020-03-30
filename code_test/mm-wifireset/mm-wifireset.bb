DESCRIPTION = "wifi reset"
HOMEPAGE = "http://support.cdmatech.com"
LICENSE = "QUALCOMM-TECHNOLOGY-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qti/files/qcom-licenses/${LICENSE};md5=400dd647645553d955b1053bbbfcd2de"
PV = "1.0"
PR = "r0"

SRC_URI += "file://wifireset.c \
	    file://Makefile \
	    file://wifireset.conf \
	   "


PACKAGES = "${PN}"
INSANE_SKIP_${PN} = "installed-vs-shipped"
INHIBIT_PACKAGE_STRIP = "1"

INSANE_SKIP_${PN} += "arch"
FILES_${PN} += "/etc/init/*"

do_unpack_append() {
    import shutil
    import os
    s = d.getVar('S', True)
    wd = d.getVar('WORKDIR',True)
    if not os.path.exists(s):
        os.makedirs(s)
    shutil.copy(wd+'/Makefile', s)
    shutil.copy(wd+'/wifireset.c', s)
    shutil.copy(wd+'/wifireset.conf', s)
}

do_install_append(){
    dest=/usr/bin
    install -d ${D}${dest}
    install -m 0755 wifireset ${D}${dest}

    inits=/etc/init
    install -d ${D}${inits}
    install -m 0755 wifireset.conf ${D}${inits}

}