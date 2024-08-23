FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI:append = " file://0001-Remove-user-ingroup-redfish.patch "

EXTRA_OEMESON:append = " \
    -Dinsecure-disable-csrf=enabled \
    -Dinsecure-disable-xss=enabled \
    -Dwerror=false \
"