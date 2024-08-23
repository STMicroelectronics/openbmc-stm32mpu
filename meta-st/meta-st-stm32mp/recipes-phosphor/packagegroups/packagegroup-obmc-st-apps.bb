SUMMARY = "OpenBMC for STM32MP boards system - Applications"
PR = "r1"

inherit packagegroup

PROVIDES = "${PACKAGES}"
PACKAGES = " \
        ${PN}-chassis \
        ${PN}-fans \
        ${PN}-flash \
        ${PN}-system \
        "
PROVIDES += "virtual/obmc-chassis-mgmt"
PROVIDES += "virtual/obmc-fan-mgmt"
PROVIDES += "virtual/obmc-flash-mgmt"
PROVIDES += "virtual/obmc-system-mgmt"

RPROVIDES:${PN}-chassis += "virtual-obmc-chassis-mgmt"
RPROVIDES:${PN}-fans += "virtual-obmc-fan-mgmt"
RPROVIDES:${PN}-flash += "virtual-obmc-flash-mgmt"
RPROVIDES:${PN}-system += "virtual-obmc-system-mgmt"

# SUMMARY:${PN}-chassis = "EVB NPCM845 Chassis"
# RDEPENDS:${PN}-chassis = " \
#         x86-power-control \
#         "

# SUMMARY:${PN}-fans = "EVB NPCM845 Fans"
# RDEPENDS:${PN}-fans = " \
#         phosphor-pid-control \
#         "

# SUMMARY:${PN}-flash = "EVB NPCM845 Flash"
# RDEPENDS:${PN}-flash = " \
#         phosphor-ipmi-flash \
#         "

SUMMARY:${PN}-system = "STM32MP2 board System"
RDEPENDS:${PN}-system =" \
    bmcweb \
    webui-vue \
    openocd \
    ethtool \
    i2c-tools \
    libgpiod-tools \
    iperf3 \
    optee-client \
    optee-test \
    "
