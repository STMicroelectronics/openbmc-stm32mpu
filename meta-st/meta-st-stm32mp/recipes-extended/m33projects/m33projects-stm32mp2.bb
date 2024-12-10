SUMMARY = "STM32MP2 Firmware examples for CM33"
LICENSE = "Apache-2.0 & MIT & BSD-3-Clause"
LIC_FILES_CHKSUM = "file://License.md;md5=82202c4eeb14eafecb91374e9f32bfcf"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI = "git://github.com/STMicroelectronics/STM32CubeMP2.git;protocol=https;branch=main"
SRCREV  = "720f5e7cf36f4b6e2046339a84bbed001bd80b06"

PV = "1.1.0"

S = "${WORKDIR}/git"

require recipes-extended/m33projects/m33projects.inc

PROJECTS_LIST_M33 = " \
    STM32MP257F-EV1/Demonstrations/USBPD_DRP_UCSI \
    STM32MP257F-EV1/Demonstrations/LowPower_SRAM_Demo \
    STM32MP257F-DK/Demonstrations/USBPD_DRP_UCSI \
    STM32MP235F-DK/Demonstrations/USBPD_DRP_UCSI \
    STM32MP215F-DK/Demonstrations/OpenAMP/OpenAMP_TTY_echo/bin \
"

PROJECTS_LIST_M0 = " \
    STM32MP257F-EV1/Demonstrations/CM0PLUS_DEMO \
"

# WARNING: You MUST put only one project on DEFAULT_COPRO_FIRMWARE per board
# If there is several project defined for the same board while you MUST have issue at runtime
# (not the correct project could be executed).
DEFAULT_COPRO_FIRMWARE = "STM32MP257F-EV1/Demonstrations/USBPD_DRP_UCSI"
DEFAULT_COPRO_FIRMWARE += "STM32MP257F-DK/Demonstrations/USBPD_DRP_UCSI"
DEFAULT_COPRO_FIRMWARE += "STM32MP235F-DK/Demonstrations/USBPD_DRP_UCSI"

# Define default board reference for M33
M33_BOARDS += " STM32MP257F-EV1 STM32MP257F-DK STM32MP235F-DK "
M33_BOARDS += " STM32MP215F-DK "
# Define default board reference for M0
M0_BOARDS  += " STM32MP257F-EV1 "
