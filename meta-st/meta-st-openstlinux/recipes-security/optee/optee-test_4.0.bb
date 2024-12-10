SUMMARY = "OP-TEE sanity testsuite"
HOMEPAGE = "https://github.com/OP-TEE/optee_test"

LICENSE = "BSD-2-Clause & GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${S}/LICENSE.md;md5=daa2bcccc666345ab8940aab1315a4fa"

SRC_URI = "git://github.com/OP-TEE/optee_test.git;protocol=https;branch=master"
SRCREV = "1c3d6be5eaa6174e3dbabf60928d15628e39b994"

SRC_URI += " \
    file://0001-no-error-deprecated-declarations.patch \
    file://1001-xtest-stats-rely-on-exported-PTA-API-header-file.patch \
    file://1002-xtest-stats-dump-clock-and-regulator-tree-to-secure-.patch \
"

PV = "4.0.0+git${SRCPV}"

S = "${WORKDIR}/git"

require optee-test.inc
