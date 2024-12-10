#
# Archiver Configuration
#
SRC_URI:append = " file://README.HOW_TO.txt "

TF_M_ARCHIVER ?= "${@bb.utils.contains('MACHINE_FEATURES', 'm33td', 'archiver', '', d)}"
inherit_defer ${TF_M_ARCHIVER}

inherit archiver_stm32mp_clean

ARCHIVER_MODE[src] = "original"
do_ar_patched[postfuncs] =+ "archiver_clean_tarball"
COPYLEFT_LICENSE_INCLUDE:append = " BSD-3* "


archiver_create_makefile_for_sdk() {
    mkdir -p ${ARCHIVER_OUTDIR}
    cat << EOF > ${ARCHIVER_OUTDIR}/Makefile.sdk
# Set default path
SRC_PATH ?= \$(PWD)
BLD_PATH ?= \$(SRC_PATH)/../build
DEPLOYDIR ?= \$(SRC_PATH)/../deploy
FIPTOOLDIR ?= \$(SRC_PATH)/..

# Set default FIP config
FIP_CONFIG ?= ${@' '.join(d for d in '${FIP_CONFIG}'.split() if not 'fastboot-' in d)}

TF_M_DEVICETREE ?= ""
DEVICETREE ?= ${TF_M_DEVICETREE}

TFM_PROFILE ?= ${TFM_PROFILE}

TFM_PLATFORMS ?= ${TFM_PLATFORMS}

# Reset default variables
LDFLAGS =
CFLAGS =
CPPFLAGS =
CC =
CPP =
AS =
AR =
LD =
NM =

EOF
}
do_ar_patched[prefuncs] += "archiver_create_makefile_for_sdk"
