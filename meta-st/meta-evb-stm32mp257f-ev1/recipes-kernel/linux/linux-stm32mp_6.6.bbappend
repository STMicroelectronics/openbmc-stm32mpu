FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

LINUX_VERSION = "6.6"
LINUX_SUBVERSION = ".78"

SRC_URI += " \
    file://${LINUX_VERSION}/${LINUX_VERSION}${LINUX_SUBVERSION}/0001-arm64-dts-st-add-default-LED-configuration-for-OBMC.patch \
    "
