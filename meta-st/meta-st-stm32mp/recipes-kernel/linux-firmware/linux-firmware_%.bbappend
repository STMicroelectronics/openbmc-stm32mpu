FILESEXTRAPATHS:prepend:stm32mpcommon := "${THISDIR}/${PN}:"

# Add calibration file
SRC_URI:append:stm32mpcommon = " git://github.com/murata-wireless/cyw-fmac-nvram.git;protocol=https;nobranch=1;name=nvram;destsuffix=nvram-murata "
SRCREV_nvram = "61b41349b5aa95227b4d2562e0d0a06ca97a6959"
SRC_URI:append:stm32mpcommon = " git://github.com/murata-wireless/cyw-fmac-fw.git;protocol=https;nobranch=1;name=murata;destsuffix=murata "
SRCREV_murata = "a80cb77798a8d57f15b7c3fd2be65553d9bd5125"
SRCREV_FORMAT = "linux-firmware-murata"

do_install:append:stm32mpcommon() {
   # ---- 43430-----
   # Install calibration file
   install -m 0644 ${WORKDIR}/nvram-murata/cyfmac43430-sdio.1DX.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.txt
   # disable Wakeup on WLAN
   sed -i "s/muxenab=\(.*\)$/#muxenab=\1/g" ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.txt
   # Install calibration file (stm32mp15)
   install -m 0644 ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.st,stm32mp157c-dk2.txt
   install -m 0644 ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.st,stm32mp157f-dk2.txt
   # Install calibration file (stm32mp13)
   install -m 0644 ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.st,stm32mp135f-dk.txt

   # Take newest murata firmware
   install -m 0644 ${WORKDIR}/murata/cyfmac43430-sdio.bin ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.bin
   install -m 0644 ${WORKDIR}/murata/cyfmac43430-sdio.1DX.clm_blob ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.clm_blob

   # Add symlinks for newest kernel compatibility
   cd ${D}${nonarch_base_libdir}/firmware/brcm/
   ln -sf brcmfmac43430-sdio.bin brcmfmac43430-sdio.st,stm32mp157c-dk2.bin
   ln -sf brcmfmac43430-sdio.bin brcmfmac43430-sdio.st,stm32mp157f-dk2.bin
   ln -sf brcmfmac43430-sdio.bin brcmfmac43430-sdio.st,stm32mp135f-dk.bin

   # ---- 43439-----
   # Install calibration file
   install -m 0644 ${WORKDIR}/nvram-murata/cyfmac43439-sdio.1YN.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.txt
   # disable Wakeup on WLAN
   sed -i "s/muxenab=\(.*\)$/#muxenab=\1/g" ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.txt
   # Install calibration file (stm32mp25)
   install -m 0644 ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.st,stm32mp257f-dk.txt
   install -m 0644 ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.st,stm32mp235f-dk.txt

   # Take newest murata firmware
   install -m 0644 ${WORKDIR}/murata/cyfmac43439-sdio.bin ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.bin
   install -m 0644 ${WORKDIR}/murata/cyfmac43439-sdio.1YN.clm_blob ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.clm_blob

   # Add symlinks for newest kernel compatibility
   cd ${D}${nonarch_base_libdir}/firmware/brcm/
   ln -sf brcmfmac43439-sdio.bin brcmfmac43439-sdio.st,stm32mp257f-dk.bin
   ln -sf brcmfmac43439-sdio.bin brcmfmac43439-sdio.st,stm32mp257f-dk-ca35tdcid-ostl.bin
   ln -sf brcmfmac43439-sdio.bin brcmfmac43439-sdio.st,stm32mp257f-dk-ca35tdcid-ostl-m33-examples.bin
   ln -sf brcmfmac43439-sdio.bin brcmfmac43439-sdio.st,stm32mp235f-dk.bin

   # ---- 4773 ----
   # Install calibration file
   install -m 0644 ${WORKDIR}/nvram-murata/cyfmac4373-sdio.2AE.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.txt
   # disable Wakeup on WLAN
   sed -i "s/muxenab=\(.*\)$/#muxenab=\1/g" ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.txt
   # Install calibration file (stm32mp25)
   install -m 0644 ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.txt ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.st,stm32mp215f-dk.txt

   # Take newest murata firmware
   install -m 0644 ${WORKDIR}/murata/cyfmac4373-sdio.2AE.bin ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.bin
   install -m 0644 ${WORKDIR}/murata/cyfmac4373-sdio.2AE.clm_blob ${D}${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.clm_blob

   # Add symlinks for newest kernel compatibility
   cd ${D}${nonarch_base_libdir}/firmware/brcm/
   ln -sf brcmfmac4373-sdio.bin brcmfmac4373-sdio.st,stm32mp215f-dk.bin

}

FILES:${PN}-bcm43430:append:stm32mpcommon = " \
  ${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.txt \
  ${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.st,stm32mp157c-dk2.* \
  ${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.st,stm32mp157f-dk2.* \
  ${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.st,stm32mp135f-dk.* \
  ${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.clm_blob \
  ${nonarch_base_libdir}/firmware/brcm/brcmfmac43430-sdio.bin \
"

RDEPENDS:${PN}-bcm43430:remove:stm32mpcommon = " ${PN}-cypress-license "

#
PACKAGES =+ "${PN}-bcm43439"
FILES:${PN}-bcm43439:stm32mpcommon = " \
    ${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.txt \
    ${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.bin \
    ${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.clm_blob \
    ${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.st,stm32mp257f-dk* \
    ${nonarch_base_libdir}/firmware/brcm/brcmfmac43439-sdio.st,stm32mp235f-dk* \
 "

FILES:${PN}-bcm4373:append:stm32mpcommon = " \
  ${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.txt \
  ${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.bin \
  ${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.clm_blob \
  ${nonarch_base_libdir}/firmware/brcm/brcmfmac4373-sdio.st,stm32mp215f-dk.* \
"

