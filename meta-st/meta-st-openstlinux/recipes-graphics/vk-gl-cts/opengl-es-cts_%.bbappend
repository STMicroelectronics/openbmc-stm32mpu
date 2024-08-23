inherit python3native

PACKAGECONFIG = "${@bb.utils.filter('DISTRO_FEATURES', 'wayland', d)}"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "file://0001-Replace-wl_shell-with-xdg_wm_base.patch"

DEPENDS += "wayland-native"

CTSDIR = "/usr/local/${BPN}"
