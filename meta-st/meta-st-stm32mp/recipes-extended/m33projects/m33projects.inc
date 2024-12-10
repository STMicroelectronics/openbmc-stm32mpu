COMPATIBLE_MACHINE = "(stm32mp2common)"

PACKAGE_ARCH = "${MACHINE_ARCH}"

B = "${S}"
# For external source support with devtools
EXTERNALSRC_BUILD:pn-${PN} = "${S}"

DEPENDS += "gcc-arm-none-eabi-native cmake-native"
DEPENDS += "copro-sdk-native virtual-optee-os"
DEPENDS += "${@oe.utils.ifelse(d.getVar('M33PROJECT_TF_M_TRUSTZONE') == '1', 'tf-m-stm32mp','')}"
# for optee script
DEPENDS += "python3-pyelftools-native python3-pycryptodomex-native python3-cryptography-native"

# Default service for systemd
inherit systemd update-rc.d python3native
SRC_URI += "file://st-m33firmware-load-default.sh"
SRC_URI += "file://st-m33firmware-load.service"
SRC_URI += "file://shutdown-stm32mp2-m33.sh"
# Temporary
SRC_URI += "file://fw_cortex_m33.sh"
SRC_URI += "file://fw_cortex_m0.sh"

# Create specific userfs package
M33_PACKAGE_4USERFS ?= "1"
PACKAGES += "${@bb.utils.contains('M33_PACKAGE_4USERFS', '1', '${PN}-userfs', '', d)}"

# Define default board reference for M33
M33_BOARDS ?= ""
# Define default board reference for M0
M0_BOARDS ?= ""

# Init M33 board service install
M33_BOARDS_SERVICE ?= "1"
# list of boards/devitree on which the service must be started
M33_BOARDS_START_AT_STARTUP ?= ""

# Init default copro example to load/execute
DEFAULT_COPRO_FIRMWARE ?= ""

BUILD_CONFIG ?= "Debug"

# Init default installation path
M33_FOLDER ?= "Cube-M33-examples"
M33_INSTALLDIR ?= "${prefix}local"
M33_OUTPUT_4USERFS = "${M33_INSTALLDIR}/${M33_FOLDER}"

export TA_DEV_KIT_DIR="${STAGING_INCDIR}/optee/export-user_ta_arm64"
export SBOOTADDR="0x80000000"
export NSBOOTADDR="0x80100000"

# M33 key settings
M33_OPTEE_KEYNAME ?= "default.pem"

M33_ELF_NOT_STRIPPED ??= "0"
M0_ELF_NOT_STRIPPED ??= "0"

do_compile() {
    unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS
    for BIN_MACHINE in ${M33_BOARDS}; do
        for project in ${PROJECTS_LIST_M33} ; do
            if [ "$(echo ${project} | cut -d'/' -f1)" = "${BIN_MACHINE}" ]; then
                if [ -d "${B}/Projects/${project}" ]; then
                    if [ $(basename ${project}) = "bin" ]; then
                        continue
                    fi
                    if [ ! -e "${B}/Projects/${project}/CMakeLists.txt" ]; then
                        continue
                    fi
                    cd "${B}/Projects/${project}"
                    rm -rf ${B}/Projects/${project}/build
                    export PATH=${STAGING_DATADIR_NATIVE}/gcc-arm-none-eabi/bin:${STAGING_BINDIR_NATIVE}:$PATH
                    # generate Makefile
                    ${STAGING_BINDIR_NATIVE}/cmake -G"Unix Makefiles" -B ${B}/Projects/${project}/build
                    # build
                    cd ${B}/Projects/${project}/build; ${STAGING_BINDIR_NATIVE}/make all
                fi
            fi
        done
    done
    for BIN_MACHINE in ${M0_BOARDS}; do
        for project in ${PROJECTS_LIST_M0} ; do
            if [ "$(echo ${project} | cut -d'/' -f1)" = "${BIN_MACHINE}" ]; then
                if [ -d "${B}/Projects/${project}" ]; then
                    if [ ! -e "${B}/Projects/${project}/CMakeLists.txt" ]; then
                        continue
                    fi
                    cd "${B}/Projects/${project}"
                    rm -rf ${B}/Projects/${project}/build
                    export PATH=${STAGING_DATADIR_NATIVE}/gcc-arm-none-eabi/bin:${STAGING_BINDIR_NATIVE}:$PATH
                    # generate Makefile
                    ${STAGING_BINDIR_NATIVE}/cmake -G"Unix Makefiles" -B ${B}/Projects/${project}/build
                    # build
                    cd ${B}/Projects/${project}/build; ${STAGING_BINDIR_NATIVE}/make all
                fi
            fi
        done
    done

}

do_install() {
    export OPENSSL_MODULES=${STAGING_LIBDIR_NATIVE}/ossl-modules/
    # Install M33 firmwares, scripts and README in userfs:
    # <userfs>/Cube-M33-examples/${project} (ie STM32MP257F-VALID3/Applications/OpenAMP/OpenAMP_TTY_echo)
    #                        |-- fw_cortex_m33.sh
    #                        |-- README
    #                        |--lib
    #                             |--firmware
    #                                  |-- ELF file for impacted board (ie OpenAMP_TTY_echo_CM33_NonSecure.elf)
    for BIN_MACHINE in ${M33_BOARDS}; do
        for project in ${PROJECTS_LIST_M33} ; do
            if [ "$(echo ${project} | cut -d'/' -f1)" = "${BIN_MACHINE}" ]; then
                 if [ -d "${B}/Projects/${project}/build" ]; then
                     BIN_NAME=$(basename ${project})
                     PROJ_DIR=${project}/build
                     INSTALL_PROJ=${project}
                 else
                     if [ -d "${B}/Projects/${project}" ]; then
                         BIN_NAME=$(basename $(dirname ${project}))
                         PROJ_DIR=${project}
                         INSTALL_PROJ=$(echo ${project} | sed 's,/bin,,')
                     else
                         bbfatal "[install] Issue with ${project}"
                     fi
                 fi
                BIN_NAME_NS=$(echo $BIN_NAME"_CM33_NonSecure")
                project_lower=$(echo $BIN_MACHINE | sed -e 's/\(.*\)/\L\1/')
                tfm_secure_binary_name=$(echo "tfm-"$project_lower"_s.elf")

                # Install M33 firmwares
                install -d ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/
                if [ "${M33_ELF_NOT_STRIPPED}" = "1" ]; then
                    install -m 0755 ${B}/Projects/${PROJ_DIR}/${BIN_NAME_NS}.elf ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/
                fi
                if [ -e ${B}/Projects/${PROJ_DIR}/${BIN_NAME_NS}_stripped.elf ]; then
                    install -m 0755 ${B}/Projects/${PROJ_DIR}/${BIN_NAME_NS}_stripped.elf ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/
                    BIN_NAME_NS_file=$(echo $BIN_NAME"_CM33_NonSecure_stripped")
                else
                    install -m 0755 ${B}/Projects/${PROJ_DIR}/${BIN_NAME_NS}.elf ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/
                    BIN_NAME_NS_file=$(echo $BIN_NAME"_CM33_NonSecure")
                fi

                # Install sh and README files if any for each example
                if [ -e ${B}/Projects/${PROJ_DIR}/fw_cortex_m33.sh ]; then
                    install -m 0755 ${B}/Projects/${PROJ_DIR}/fw_cortex_m33.sh ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}
                fi
                if [ -e ${S}/Projects/${project}/Remoteproc/README ]; then
                    install -m 0755 ${S}/Projects/${project}/Remoteproc/README ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}
                fi

                # generate signed binaries
                # generate ${BIN_NAME_NS}_sign.bin
                # Get isr_vectors from elf file
                elf_NSBOOTADDR=$(${READELF} -S ${B}/Projects/${PROJ_DIR}/${BIN_NAME_NS}.elf | grep isr_vectors | awk '{print($5)}')
                NSBOOTADDR=0x${elf_NSBOOTADDR} ${STAGING_BINDIR_NATIVE}/st_copro_firmware_signature.sh \
                        --input-nsecure ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/${BIN_NAME_NS_file}.elf \
                        --signature-key ${TA_DEV_KIT_DIR}/keys/${M33_OPTEE_KEYNAME} \
                        --output ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/${BIN_NAME_NS}
                if [ -e ${RECIPE_SYSROOT}/firmware/$tfm_secure_binary_name ]; then
                    # generate ${BIN_NAME_NS}_tfm_sign.bin
                    ${STAGING_BINDIR_NATIVE}/st_copro_firmware_signature.sh \
                            --input-nsecure ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/${BIN_NAME_NS_file}.elf \
                            --input-secure ${RECIPE_SYSROOT}/firmware/$tfm_secure_binary_name \
                            --signature-key ${TA_DEV_KIT_DIR}/keys/${M33_OPTEE_KEYNAME} \
                            --output ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/${BIN_NAME_NS}
                else
                    bbwarn "The TFM firmware doesn't exist (file=$tfm_secure_binary_name)"
                fi
                #temporary: add firmware to permit to load the project at runtime
                install -m 0755 ${WORKDIR}/fw_cortex_m33.sh ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/
                for ex in ${DEFAULT_COPRO_FIRMWARE}; do
                    if [ "${project}" = "${ex}" ]; then
                        board_project=$(echo ${project} | cut -d'/' -f1 | tr '[:upper:]' '[:lower:]')
                        for sb in ${M33_BOARDS_START_AT_STARTUP}; do
                            if $(echo ${sb} | grep -q ${board_project}) ; then
                                echo "${ex}" > ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/default.${sb}
                            fi
                        done
                    fi
                done
            fi
        done
    done
    for BIN_MACHINE in ${M0_BOARDS}; do
        for project in ${PROJECTS_LIST_M0} ; do
            if [ "$(echo ${project} | cut -d'/' -f1)" = "${BIN_MACHINE}" ]; then
                 if [ -d "${B}/Projects/${project}/build" ]; then
                     BIN_NAME=$(basename ${project})
                     PROJ_DIR=${project}/build
                     INSTALL_PROJ=${project}
                 else
                     if [ -d "${B}/Projects/${project}" ]; then
                         BIN_NAME=$(basename $(dirname ${project}))
                         PROJ_DIR=${project}
                         INSTALL_PROJ=$(echo ${project} | sed 's,/bin,,')
                     else
                         bbfatal "[install M0] Issue with ${project}"
                     fi
                 fi
                project_lower=$(echo $BIN_MACHINE | sed -e 's/\(.*\)/\L\1/')
                BIN_NAME_NS=$(echo $BIN_NAME"_NonSecure")

                # Install M0 firmwares
                install -d ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/
                install -m 0755 ${B}/Projects/${PROJ_DIR}/${BIN_NAME_NS}_stripped.elf ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/
                install -m 0755 ${B}/Projects/${PROJ_DIR}/${BIN_NAME_NS}.bin ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/
                if [ "${M0_ELF_NOT_STRIPPED}" = "1" ]; then
                    install -m 0755 ${B}/Projects/${PROJ_DIR}/${BIN_NAME_NS}.elf ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/
                fi
                # Install sh and README files if any for each example
                install -m 0755 ${WORKDIR}/fw_cortex_m0.sh ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/
            fi
        done
    done

    if [ "${M33_BOARDS_SERVICE}" -eq 1 ]; then
        # Install sh script to load/execute DEFAULT_COPRO_FIRMWARE @boot up
        if [ -n "${DEFAULT_COPRO_FIRMWARE}" ]; then
            # Install systemd service
            install -d ${D}${sysconfdir}/init.d/
            install -d ${D}${base_sbindir}/
            install -m 0755 ${WORKDIR}/st-m33firmware-load-default.sh ${D}${sysconfdir}/init.d/st-m33firmware-load-default.sh
            install -m 0755 ${WORKDIR}/st-m33firmware-load-default.sh ${D}${base_sbindir}/st-m33firmware-load-default.sh
            install -m 0755 ${WORKDIR}/shutdown-stm32mp2-m33.sh ${D}${base_sbindir}/shutdown-stm32mp2-m33.sh

            sed -i -e "s:##BOARDS_LIST##:${M33_BOARDS_START_AT_STARTUP}:g" \
            ${D}${sysconfdir}/init.d/st-m33firmware-load-default.sh
            sed -i -e "s:##BOARDS_LIST##:${M33_BOARDS_START_AT_STARTUP}:g" \
            ${D}${base_sbindir}/st-m33firmware-load-default.sh

            sed -i -e "s:@userfs_mount_point@:${M33_OUTPUT_4USERFS}:g" \
            ${D}${sysconfdir}/init.d/st-m33firmware-load-default.sh
            sed -i -e "s:@userfs_mount_point@:${M33_OUTPUT_4USERFS}:g" \
            ${D}${base_sbindir}/st-m33firmware-load-default.sh
        fi

        # Install systemd service for all machines configurations
        if ${@bb.utils.contains('DISTRO_FEATURES','systemd','true','false',d)}; then
            install -d ${D}${systemd_unitdir}/system
            install -d ${D}${systemd_unitdir}/system-shutdown
            install -m 644 ${WORKDIR}/st-m33firmware-load.service ${D}/${systemd_unitdir}/system
            install -m 755 ${WORKDIR}/shutdown-stm32mp2-m33.sh ${D}/${systemd_unitdir}/system-shutdown
        fi
    fi
}



# -----------------------------------------------------------
# specific for service: start copro m33 firwmare at boot time
SYSTEMD_PACKAGES += " m33projects-stm32mp2 "
SYSTEMD_SERVICE:${PN} = "st-m33firmware-load.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

INITSCRIPT_NAME = "st-m33firmware-load-default.sh"
INITSCRIPT_PARAMS = "stop 22 5 3 ."
# -----------------------------------------------------------

INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_SYSROOT_STRIP = "1"

FILES:${PN} += "${nonarch_base_libdir}/firmware ${sysconfdir}/init.d ${systemd_unitdir}/system ${systemd_unitdir}/system-shutdown"
# Configure package split
FILES:${PN} += "${@bb.utils.contains('M33_PACKAGE_4USERFS', '1', '', '${M33_OUTPUT_4USERFS}', d)}"
FILES:${PN}-userfs = "${@bb.utils.contains('M33_PACKAGE_4USERFS', '1', '${M33_OUTPUT_4USERFS}', '', d)}"
RDEPENDS:${PN}-userfs += "busybox"

# Avoid QA issue because binaries are for an Arm architecture but the platform is an AArch64
INSANE_SKIP = "arch"
INSANE_SKIP:${PN} = "already-stripped buildpaths"
INSANE_SKIP:${PN}-userfs = "buildpaths"
INSANE_SKIP:${PN}-dbg = "buildpaths"
