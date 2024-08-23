# We don't want etnaviv drm package
PACKAGECONFIG:stm32mpcommon = "install-test-programs tests"

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}/:"

SRC_URI:append:stm32mpcommon = " \
        file://0001-tests-modetest-automatic-configuration.patch \
        file://0002-tests-util-smtpe-increase-alpha-to-middle-band.patch \
        file://0003-tests-modetest-set-property-in-atomic-mode.patch \
        file://0004-tests-modetest-close-crtc.patch \
        "
