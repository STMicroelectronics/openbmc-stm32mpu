#
# Archiver Configuration
#
SRC_URI:append = " file://README.HOW_TO.txt "

inherit archiver
ARCHIVER_MODE[src] = "original"
COPYLEFT_LICENSE_INCLUDE:append = " BSD-3* "

inherit archiver_stm32mp_clean

archiver_create_makefile_for_sdk() {
    mkdir -p ${ARCHIVER_OUTDIR}

    cat << EOF > ${ARCHIVER_OUTDIR}/Makefile.sdk
# Set default path
SRC_PATH ?= \$(PWD)
BLD_PATH ?= \$(SRC_PATH)/../build
DEPLOYDIR ?= \$(SRC_PATH)/../deploy
FIPTOOLDIR ?= \$(SRC_PATH)/..

# Set default FIP config
FIP_CONFIG ?= ${@' '.join(d for d in '${FIP_CONFIG}'.split() if not 'fastboot-' in d)}

# Set default optee-os config
OPTEE_CONFIG ?= ${OPTEE_CONFIG}

EOF
    unset i
    for config in ${OPTEE_CONFIG}; do
        i=$(expr $i + 1)
        cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk
OPTEE_DEVICETREE_INTERNAL_$config ?= $(echo ${OPTEE_DEVICETREE_INTERNAL} | cut -d',' -f${i} | tr ' ' '\n' | sort -u | tr '\n' ' ')
OPTEE_DEVICETREE_EXTERNAL_$config ?= $(echo ${OPTEE_DEVICETREE_EXTERNAL} | cut -d',' -f${i} | tr ' ' '\n' | sort -u | tr '\n' ' ')
OPTEE_DEVICETREE_$config = \$(OPTEE_DEVICETREE_INTERNAL_${config}) \$(if \$(EXTDT_DIR),\$(OPTEE_DEVICETREE_EXTERNAL_${config}))
OPTEE_EXTRA_OPTFLAGS_$config ?= $(echo ${OPTEE_EXTRA_OPTFLAGS} | cut -d',' -f${i})
EOF
    done

cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk

# use the following variable to limit the devicetree
CFG_EMBED_DTB_SOURCE_FILE ?=

# Remove default variables
LDFLAGS =
CFLAGS = \$(LIBGCC_LOCATE_CFLAGS)
CPPFLAGS =
# Define default make options
EXTRA_OEMAKE = $(echo "${EXTRA_OEMAKE}" | sed "s|LIBGCC_LOCATE_CFLAGS=[^ ]* |LIBGCC_LOCATE_CFLAGS=\$(KCFLAGS) |" | \
                sed 's,CFG_EXT_DTS='"${STAGING_EXTDT_DIR}/${EXTDT_DIR_OPTEE}"',CFG_EXT_DTS=\$(EXTDT_DIR)/'"${EXTDT_DIR_OPTEE}"',')

help:
	@echo
	@echo "OPTEE-OS configuration:"
	@echo "  CFG_EMBED_DTB_SOURCE_FILE = \$(CFG_EMBED_DTB_SOURCE_FILE)"
	@echo
	@echo "OPTEE-OS folder configuration:"
	@echo "  SRC_PATH  = \$(SRC_PATH)"
	@echo "  BLD_PATH  = \$(BLD_PATH)"
	@echo "  DEPLOYDIR = \$(DEPLOYDIR)"
	@echo "  EXTDT_DIR = \$(EXTDT_DIR)"
	@echo
	@echo "FIP configuration:"
	@echo "  Do not forget to set FIP deploydir folders (such as FIP_DEPLOYDIR_ROOT) to provide path to needed binaries"
	@echo
	@echo "Available targets:"
	@echo "  all   : default target to build all binaries for defined config(s)"
	@echo "  fip   : build FIP binaries for defined config(s)"
	@echo "  optee : build OPTEE-OS binaries for defined config(s)"
	@echo "  clean : clean build directories from generated files"
	@echo

# optee-<CONFIG>-<SOC>-<DT>
define optee-rules-by-dt
tee-\$(1)-\$(2)-\$(3):
	@mkdir -p  \$(BLD_PATH)/\$(1)-\$(3); \\
	touch \$(BLD_PATH)/\$(1)-\$(3)/\$(3)-sysram.txt ; \\
	if [ "\$(2)" = "stm32mp15" ] && [ "\$(1)" = "optee" ]; then \\
		echo "CFG_STM32MP1_OPTEE_IN_SYSRAM=y" > \$(BLD_PATH)/\$(1)-\$(3)/\$(3)-sysram.txt; \\
	fi; \\
	\$(MAKE) \$(EXTRA_OEMAKE) \\
		\$(OPTEE_EXTRA_OPTFLAGS_\$(1)) \\
		-C \$(SRC_PATH) PREFIX=\$(SDKTARGETSYSROOT) \\
		O=\$(BLD_PATH)/\$(1)-\$(3) \\
		CFG_EMBED_DTB_SOURCE_FILE=\$(3).dts \\
		\`cat \$(BLD_PATH)/\$(1)-\$(3)/\$(3)-sysram.txt\` \\
		\$(shell echo CFG_\$(2) | tr a-z A-Z)=y || exit 1;
endef

# deploy-<config>-<soc>-<dt>
define deploy-rules-by-dt
deploy-\$(1)-\$(2)-\$(3): tee-\$(1)-\$(2)-\$(3)
	@mkdir -p \$(DEPLOYDIR)
	@mkdir -p \$(DEPLOYDIR)/debug
	\$(if \$(findstring \$(2),\$(3)), \\
		cp -f \$(BLD_PATH)/\$(1)-\$(3)/core/tee-header_v2.bin \$(DEPLOYDIR)/tee-header_v2-\$(3)-\$(1).bin ; \\
		cp -f \$(BLD_PATH)/\$(1)-\$(3)/core/tee-pager_v2.bin \$(DEPLOYDIR)/tee-pager_v2-\$(3)-\$(1).bin ; \\
		cp -f \$(BLD_PATH)/\$(1)-\$(3)/core/tee-pageable_v2.bin \$(DEPLOYDIR)/tee-pageable_v2-\$(3)-\$(1).bin ; \\
		cp -f \$(BLD_PATH)/\$(1)-\$(3)/core/tee.elf \$(DEPLOYDIR)/debug/tee-\$(3)-\$(1).elf ; \\
	,)
endef

# generate optee rules by dt
\$(foreach config, \$(OPTEE_CONFIG), \\
	\$(foreach soc,  ${STM32MP_SOC_NAME} , \\
		\$(foreach dt, \$(if \$(CFG_EMBED_DTB_SOURCE_FILE), \$(CFG_EMBED_DTB_SOURCE_FILE),\$(OPTEE_DEVICETREE_\$(config))), \\
			\$(if \$(findstring \$(soc),\$(dt)), \\
				\$(eval \$(call optee-rules-by-dt,\$(config),\$(soc),\$(dt))) \\
			,)\\
		)\\
	)\\
)

# generate deploy optee rules by dt
\$(foreach config, \$(OPTEE_CONFIG), \\
	\$(foreach soc, ${STM32MP_SOC_NAME} , \\
		\$(foreach dt, \$(if \$(CFG_EMBED_DTB_SOURCE_FILE), \$(CFG_EMBED_DTB_SOURCE_FILE),\$(OPTEE_DEVICETREE_\$(config))), \\
			\$(if \$(findstring \$(soc),\$(dt)), \\
				\$(eval \$(call deploy-rules-by-dt,\$(config),\$(soc),\$(dt))) \\
			,)\\
		)\\
	)\\
)

# Configure overall deploy rules list
optee-deploy := \$(foreach config, \$(OPTEE_CONFIG), \$(foreach soc, ${STM32MP_SOC_NAME}, \$(foreach dt, \$(if \$(CFG_EMBED_DTB_SOURCE_FILE), \$(CFG_EMBED_DTB_SOURCE_FILE),\$(OPTEE_DEVICETREE_\$(config))), \$(if \$(findstring \$(soc),\$(dt)), deploy-\$(config)-\$(soc)-\$(dt),)  )))

optee-compile:= \$(foreach config, \$(OPTEE_CONFIG), \$(foreach soc, ${STM32MP_SOC_NAME}, \$(foreach dt, \$(if \$(CFG_EMBED_DTB_SOURCE_FILE), \$(CFG_EMBED_DTB_SOURCE_FILE),\$(OPTEE_DEVICETREE_\$(config))), \$(if \$(findstring \$(soc),\$(dt)), tee-\$(config)-\$(soc)-\$(dt),) )))

optee: \$(optee-compile) \$(optee-deploy)

fip: \$(optee-deploy)
	for fipconfig in \$(FIP_CONFIG) ; do \\
		FIP_DEPLOYDIR_OPTEE=\$(DEPLOYDIR) FIP_CONFIG="\$\$fipconfig" \$(if \$(CFG_EMBED_DTB_SOURCE_FILE),FIP_DEVICETREE="\$(CFG_EMBED_DTB_SOURCE_FILE)") \$(FIPTOOLDIR)/fiptool-stm32mp || exit 1;\\
	done

# Set dependencies list for building all
DEPS = \$(optee-deploy)
DEPS += fip

all: \$(DEPS)

clean:
	@echo "Removing \$(BLD_PATH) ..."
	@rm -rf \$(BLD_PATH)
	@echo "Removing \$(DEPLOYDIR) ..."
	@rm -rf \$(DEPLOYDIR)
	@echo
EOF
}
do_ar_original[prefuncs] += "archiver_create_makefile_for_sdk"
