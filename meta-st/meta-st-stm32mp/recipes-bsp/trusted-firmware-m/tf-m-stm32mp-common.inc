FILESEXTRAPATHS:prepend := "${THISDIR}/tf-m-stm32mp:"

PACKAGE_ARCH = "${MACHINE_ARCH}"

CVE_PRODUCT = "arm:trusted_firmware-m"

inherit python3native deploy external-dt

# Baremetal and we bring a compiler below
INHIBIT_DEFAULT_DEPS = "1"

DEPENDS += "cmake-native \
            ninja-native \
            gcc-arm-none-eabi-native \
            python3-cbor2-native \
            python3-click-native \
            python3-cryptography-native \
            python3-pyasn1-native \
            python3-imgtool-native \
            python3-jinja2-native \
            python3-pyyaml-native \
            python3-pyelftools-native \
"

S = "${WORKDIR}/git"
B = "${WORKDIR}/build"

# Configure TFM external sources
TFM_EXTERNAL_SOURCES ??= "1"

# Platform must be set, ideally in the machine configuration.
TFM_PLATFORMS ?= "stm/stm32mp257f_ev1"
python() {
    if not d.getVar("TFM_PLATFORMS"):
        raise bb.parse.SkipRecipe("TFM_PLATFORMS needs to be set")
}

# Build for debug (set TFM_DEBUG to 1 to activate)
TFM_DEBUG ?= "${@bb.utils.contains('MACHINE_FEATURES', 'm33td', '0', '1', d)}"

PACKAGECONFIG ??= ""
# Whether to integrate the test suite
PACKAGECONFIG[test-secure] = "-DTEST_S=ON,-DTEST_S=OFF"
PACKAGECONFIG[test-nonsecure] = "-DTEST_NS=ON,-DTEST_NS=OFF"
PACKAGECONFIG[test-psa] = "-DTEST_PSA_API=ON,-DTEST_PSA_API=OFF"

# Currently we only support using the Arm binary GCC
EXTRA_OECMAKE += "-DTFM_TOOLCHAIN_FILE=${S}/toolchain_GNUARM.cmake"

# Don't let FetchContent download more sources during do_configure
EXTRA_OECMAKE += "${@bb.utils.contains('TFM_EXTERNAL_SOURCES', '1', '-DFETCHCONTENT_FULLY_DISCONNECTED=ON', '', d)}"

# Handle TFM_DEBUG parameter
EXTRA_OECMAKE += "${@bb.utils.contains('TFM_DEBUG', '1', '-DCMAKE_BUILD_TYPE=Debug', '-DCMAKE_BUILD_TYPE=Relwithdebinfo', d)}"

# Verbose builds
EXTRA_OECMAKE += "-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON"

# Path to extra components
EXTRA_OECMAKE += "${@bb.utils.contains('TFM_EXTERNAL_SOURCES', '1', '-DMBEDCRYPTO_PATH=${S}/external/mbedtls ', '', d)}"
EXTRA_OECMAKE += "${@bb.utils.contains('TFM_EXTERNAL_SOURCES', '1', '-DTFM_TEST_REPO_PATH=${S}/external/tf-m-tests', '', d)}"
EXTRA_OECMAKE += "${@bb.utils.contains('TFM_EXTERNAL_SOURCES', '1', '-DMCUBOOT_PATH=${S}/external/mcuboot -DQCBOR_PATH=${S}/external/qcbor', '', d)}"
EXTRA_OECMAKE += "${@bb.utils.contains('TFM_EXTERNAL_SOURCES', '1', '-DDDR_PHY_BIN_SRC_PATH=${S}/external/stm32-ddr-phy-binary ', '', d)}"

# SCP firmware
SCP_FIRMWARE_OECMAKE = "-DSCP_FW_PATH=${S}/external/scp-firmware"
EXTRA_OECMAKE += " ${SCP_FIRMWARE_OECMAKE}"

# M33TD
EXTRA_OECMAKE += "${@bb.utils.contains('MACHINE_FEATURES', 'm33td', '-DSTM32_M33TDCID=ON', '-DSTM32_M33TDCID=OFF', d)}"

# TFM Profile: medium
TFM_PROFILE ??= "profile_medium"
EXTRA_OECMAKE += " ${@'-DTFM_PROFILE=${TFM_PROFILE}' if ((d.getVar('TFM_PROFILE') or '') != '') else ''}"

export CMAKE_BUILD_PARALLEL_LEVEL = "${@oe.utils.parallel_make(d, False)}"

# Let the Makefile handle setting up the CFLAGS and LDFLAGS as it is a standalone application
CFLAGS[unexport] = "1"
LDFLAGS[unexport] = "1"
AS[unexport] = "1"
LD[unexport] = "1"


# python3-cryptography needs the legacy provider, so set OPENSSL_MODULES to the
# right path until this is relocated automatically.
export OPENSSL_MODULES="${STAGING_LIBDIR_NATIVE}/ossl-modules"

# Configure for external source download directly from cmake compile
do_compile[network] = "${@bb.utils.contains('TFM_EXTERNAL_SOURCES', '1', '0', '1', d)}"
do_compile:prepend() {
    if [ "${TFM_EXTERNAL_SOURCES}" -eq "0" ]; then
        # Used to download cmake dependencies when behind a proxy
        if [ -n "${http_proxy}" ]; then
            export HTTP_PROXY=${http_proxy}
        fi
        if [ -n "${https_proxy}" ]; then
            export HTTPS_PROXY=${https_proxy}
        fi
    fi
}

# TF-M ships patches that it needs applied to mbedcrypto, so apply them
# as part of do_patch.
# Need to apply patches in submodule to avoid failure with 'devtool modify'
apply_local_patches() {
    cd ${S}/external/mbedtls
    for fp in ${S}/lib/ext/mbedcrypto/*.patch; do
        git am ${fp}
    done
    cd -
}
do_patch[postfuncs] += "${@bb.utils.contains('TFM_EXTERNAL_SOURCES', '1', 'apply_local_patches', '', d)}"

do_configure() {
    :
}

# Invoke install here as there's no point in splitting compile from install: the
# first thing the build does is 'install' inside the build tree thus causing a
# rebuild. It also overrides the install prefix to be in the build tree, so you
# can't use the usual install prefix variables.
TF_M_DTS_SUB_PATH="platform/ext/target/stm/common/devicetree/dts/arm/stm"

EXTRA_OECMAKE_NS ?= "${@bb.utils.contains('MACHINE_FEATURES','m33td','-DNS=ON','-DNS=OFF',d)} "

do_compile() {
    export PATH=$PATH:${STAGING_DATADIR_NATIVE}/gcc-arm-none-eabi/bin
    for platform in ${TFM_PLATFORMS};
    do
        for dt in ${TF_M_DEVICETREE}; do
            export PATH=$PATH:${STAGING_DATADIR_NATIVE}/gcc-arm-none-eabi/bin
            EXTRA_OECMAKE_BL2=""
            if ${@bb.utils.contains('MACHINE_FEATURES','m33td','true','false',d)}; then
                EXTRA_OECMAKE_BL2="-DDTS_BOARD_BL2=${dt}-bl2.dts"
            fi
            platform_path=$(echo "$(basename $platform)_${dt}")
            # strategy
            # test devicetree exits: use it
            # else
            #    * devicetree not exist *
            #    test devicetree with extenstion _s.dts exist: use it
            #    else
            #       test is external dt
            #           if devicetree on external dt exist: use it
            #           else
            #              use platform
            #       else
            #           use platform
            if [ -e ${S}/${TF_M_DTS_SUB_PATH}/${dt}.dts ]; then
                echo "cmake -GNinja \
                    -S ${S} -B ${B}/$platform_path \
                    ${EXTRA_OECMAKE} \
                    -DTFM_PLATFORM=$platform \
                    -DDTS_BOARD_NS=${dt}-ns.dts \
                    -DDTS_BOARD_S=${dt}.dts \
                    ${EXTRA_OECMAKE_BL2} \
                    -DDTS_EXT_DIR=${S}/${TF_M_DTS_SUB_PATH}/ \
                    ${EXTRA_OECMAKE_NS} \
                    ${PACKAGECONFIG_CONFARGS}"
                cmake -GNinja \
                    -S ${S} -B ${B}/$platform_path \
                    ${EXTRA_OECMAKE} \
                    -DTFM_PLATFORM=$platform \
                    -DDTS_BOARD_NS=${dt}-ns.dts \
                    -DDTS_BOARD_S=${dt}.dts \
                    ${EXTRA_OECMAKE_BL2} \
                    -DDTS_EXT_DIR=${S}/${TF_M_DTS_SUB_PATH}/ \
                    ${EXTRA_OECMAKE_NS} \
                    ${PACKAGECONFIG_CONFARGS}
                cmake --build ${B}/$platform_path -- install
            else
                if [ -e ${S}/${TF_M_DTS_SUB_PATH}/${dt}-s.dts ]; then
                    echo "cmake -GNinja \
                        -S ${S} -B ${B}/$platform_path \
                        ${EXTRA_OECMAKE} \
                        -DTFM_PLATFORM=$platform \
                        -DDTS_BOARD_NS=${dt}-ns.dts \
                        -DDTS_BOARD_S=${dt}-s.dts \
                        ${EXTRA_OECMAKE_BL2} \
                        -DDTS_EXT_DIR=${S}/${TF_M_DTS_SUB_PATH}/ \
                        ${EXTRA_OECMAKE_NS} \
                        ${PACKAGECONFIG_CONFARGS}"
                    cmake -GNinja \
                        -S ${S} -B ${B}/$platform_path \
                        ${EXTRA_OECMAKE} \
                        -DTFM_PLATFORM=$platform \
                        -DDTS_BOARD_NS=${dt}-ns.dts \
                        -DDTS_BOARD_S=${dt}-s.dts \
                        ${EXTRA_OECMAKE_BL2} \
                        -DDTS_EXT_DIR=${S}/${TF_M_DTS_SUB_PATH}/ \
                        ${EXTRA_OECMAKE_NS} \
                        ${PACKAGECONFIG_CONFARGS}
                    cmake --build ${B}/$platform_path -- install
                else
                    if ${@bb.utils.contains('EXTERNAL_DT_ENABLED','1','true','false',d)}; then
                        #FIXME need alignment between a35 and m33 namming
                        EXTDT_DTS_S="${dt}.dts"
                        if ${@bb.utils.contains('MACHINE_FEATURES','m33td','true','false',d)}; then
                            EXTDT_DTS_S="${dt}-s.dts"
                        fi
                        if  [ -e ${STAGING_EXTDT_DIR}/${EXTDT_DIR_TF_M}/${EXTDT_DTS_S} ]; then
                            echo "cmake -GNinja \
                                -S ${S} -B ${B}/$platform_path \
                                ${EXTRA_OECMAKE} \
                                -DTFM_PLATFORM=$platform \
                                -DDTS_BOARD_NS=${dt}-ns.dts \
                                -DDTS_BOARD_S=${EXTDT_DTS_S} \
                                ${EXTRA_OECMAKE_BL2} \
                                -DDTS_EXT_DIR=${STAGING_EXTDT_DIR}/${EXTDT_DIR_TF_M} \
                                ${EXTRA_OECMAKE_NS} \
                                ${PACKAGECONFIG_CONFARGS}"
                            cmake -GNinja \
                                -S ${S} -B ${B}/$platform_path \
                                ${EXTRA_OECMAKE} \
                                -DTFM_PLATFORM=$platform \
                                -DDTS_BOARD_NS=${dt}-ns.dts \
                                -DDTS_BOARD_S=${EXTDT_DTS_S} \
                                ${EXTRA_OECMAKE_BL2} \
                                -DDTS_EXT_DIR=${STAGING_EXTDT_DIR}/${EXTDT_DIR_TF_M} \
                                ${EXTRA_OECMAKE_NS} \
                                ${PACKAGECONFIG_CONFARGS}
                            cmake --build ${B}/$platform_path -- install
                        else
                            bbwarn "TF-M:EXTERNAL-DT: devicetree not found (${EXTDT_DTS_S}) NO TF-M binaries generated"
                        fi
                    else
                        bbwarn "TF-M: devicetree not found (${dt}.dts) NO TF-M binaries generated"
                    fi
                fi
            fi
        done
    done
}

do_compile[progress] = "outof:^\[(\d+)/(\d+)\]\s+"

do_install() {
    # TODO install headers and static libraries when we know how they're used
    # TODO: need to install only binary needed by platform
    install -d -m 755 ${D}/firmware
    for platform in ${TFM_PLATFORMS};
    do
        for dt in ${TF_M_DEVICETREE}; do
            dt_plat=$(echo ${dt} | sed "s/-/_/g")
            platform_path=$(echo "$(basename $platform)_${dt}")
            for file in $(ls -1 ${B}/$platform_path/bin/tf*);
            do
                file_name=$(basename $file)
                file_renamed=$(echo $file_name | sed "s/tfm/tfm-${dt}/")
                install -m 0644 ${B}/$platform_path/bin/$file_name ${D}/firmware/$file_renamed
            done
            for file in $(ls -1 ${B}/$platform_path/bin/bl2*);
            do
                file_name=$(basename $file)
                file_renamed=$(echo $file_name | sed "s/bl2/bl2-${dt}/")
                install -m 0644 ${B}/$platform_path/bin/$file_name ${D}/firmware/$file_renamed
            done
            for file in $(ls -1 ${B}/$platform_path/bin/ddr*);
            do
                file_name=$(basename $file)
                file_renamed=$(echo $file_name | sed "s/ddr_phy/ddr_phy-${dt}/")
                install -m 0644 ${B}/$platform_path/bin/$file_name ${D}/firmware/$file_renamed
            done
        done
    done
}

FILES:${PN} = "/firmware"
SYSROOT_DIRS += "/firmware"

addtask deploy after do_install
do_deploy() {
    install -d ${DEPLOYDIR}/arm-trusted-firmware-m
    if [ $(find ${D}/firmware/ -type f | wc -l) -gt 0 ]; then
        cp -rf ${D}/firmware/* ${DEPLOYDIR}/arm-trusted-firmware-m
    fi
}

# Build paths are currently embedded
INSANE_SKIP:${PN} += "buildpaths"
INSANE_SKIP:${PN} += "arch"
