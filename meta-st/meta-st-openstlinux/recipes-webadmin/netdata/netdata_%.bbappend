FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}/:"

SRC_URI:append = " file://python.d.conf "
SRC_URI:append = " file://kill_netdata "
SRC_URI:append = " file://gpu.chart.py "
SRC_URI:append:stm32mp1common = " file://stm32mp15.html "
SRC_URI:append:stm32mp1common = " file://stm32mp13.html "
SRC_URI:append:stm32mp2common = " file://stm32mp21.html "
SRC_URI:append:stm32mp2common = " file://stm32mp23.html "
SRC_URI:append:stm32mp2common = " file://stm32mp25.html "
SRC_URI:append:stm32mp2common = " file://fps.chart.py"

RDEPENDS:${PN}:append = " python3-multiprocessing "

do_install:append() {
    install -d ${D}${sysconfdir}/netdata
    install -d ${D}${datadir}/netdata/web
    install -d ${D}${bindir}/netdata/web

    install -m 0644 ${WORKDIR}/python.d.conf ${D}${sysconfdir}/netdata/
    install -m 0755 ${WORKDIR}/kill_netdata ${D}${bindir}/

    install -d ${D}${libexecdir}/netdata/python.d/
    install -m 0644 ${WORKDIR}/gpu.chart.py ${D}${libexecdir}/netdata/python.d/
}

do_install:append:stm32mp1common() {
    install -d ${D}${datadir}/netdata/web

    install -m 0644 ${WORKDIR}/stm32mp15.html ${D}${datadir}/netdata/web/
    install -m 0644 ${WORKDIR}/stm32mp13.html ${D}${datadir}/netdata/web/
}

do_install:append:stm32mp2common() {
    install -d ${D}${datadir}/netdata/web

    install -m 0644 ${WORKDIR}/stm32mp21.html ${D}${datadir}/netdata/web/
    install -m 0644 ${WORKDIR}/stm32mp23.html ${D}${datadir}/netdata/web/
    install -m 0644 ${WORKDIR}/stm32mp25.html ${D}${datadir}/netdata/web/

    install -d ${D}${libexecdir}/netdata/python.d/
    install -m 0644 ${WORKDIR}/fps.chart.py ${D}${libexecdir}/netdata/python.d/
}
