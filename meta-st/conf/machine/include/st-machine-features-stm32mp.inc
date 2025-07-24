#@DESCRIPTION: Machine features definitions for STM32 systems

# =========================================================================
# optee
# =========================================================================
OPTEE_LIST ?= "\
    virtual-optee-os \
    "
OPTEE_IMAGE_INSTALL = "${@bb.utils.contains('COMBINED_FEATURES', 'optee', '${OPTEE_LIST}', '', d)} "

OPTEE_BINARY ?= "\
    virtual-optee-os \
    "
OPTEE_BINARY_INSTALL = "${@bb.utils.contains('MACHINE_FEATURES', 'optee', '${OPTEE_BINARY}', '', d)} "

# =========================================================================
# pci
# =========================================================================
PCI_LIST ?= ""
PCI_IMAGE_INSTALL = "${@bb.utils.contains('MACHINE_FEATURES', 'pci', '${PCI_LIST}', '', d)} "

# =========================================================================
# Image appends
# =========================================================================
EXTRA_IMAGEDEPENDS:append = " \
    ${OPTEE_BINARY_INSTALL} \
    "

MACHINE_EXTRA_RRECOMMENDS:append = " \
    ${OPTEE_IMAGE_INSTALL}      \
    ${PCI_IMAGE_INSTALL}        \
    "
