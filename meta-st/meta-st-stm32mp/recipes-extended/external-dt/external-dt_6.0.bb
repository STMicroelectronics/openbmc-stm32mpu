SUMMARY = "Provides Device Tree files for STM32MP boards"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = "git://github.com/STMicroelectronics/dt-stm32mp.git;protocol=https;branch=v6.0-stm32mp"
SRCREV = "f6fbde7bd893091b742f8bc406ddfb51bd903739"

S = "${WORKDIR}/git"

EXT_DT_VERSION = "v6.0"
EXT_DT_RELEASE = "stm32mp-r1"

COMPATIBLE_MACHINE = "(stm32mpcommon)"

require external-dt-common.inc

# ---------------------------------
# Configure archiver use
# ---------------------------------
include ${@oe.utils.ifelse(d.getVar('ST_ARCHIVER_ENABLE') == '1', 'external-dt-archiver.inc','')}
