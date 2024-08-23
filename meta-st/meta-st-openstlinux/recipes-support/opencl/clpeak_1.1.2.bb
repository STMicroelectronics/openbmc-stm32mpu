SUMMARY = "OpenCL benchmarking application"
DESCRIPTION = "Synthetic benchmarking tool to measure peak capabilities of OpenCL devices"

LICENSE = "Unlicense"
LIC_FILES_CHKSUM = "file://LICENSE;md5=911690f51af322440237a253d695d19f"

SRC_URI = "git://github.com/krrishnarraj/clpeak.git;protocol=https;branch=master"

PV = "1.0+git${SRCPV}"
SRCREV = "a4d617d7d054450aa83965d0fd9d62ee2009957f"

DEPENDS = "virtual/opencl-icd opencl-headers opencl-clhpp"

S = "${WORKDIR}/git"

inherit cmake
