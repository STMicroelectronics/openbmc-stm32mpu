inherit image_types

IMAGE_U_BOOT_ENV_NAME = "${IMAGE_NAME}_u-boot-env-image"

write_ubi_uboot_env_config() {
	local vname="$1"

	cat <<EOF > ubinize${vname}-${IMAGE_U_BOOT_ENV_NAME}.cfg
[U-Boot-env-1]
mode=ubi
vol_id=0
vol_type=dynamic
vol_name=uboot_config
vol_size=256KiB
[U-Boot-env-2]
mode=ubi
vol_id=1
vol_type=dynamic
vol_name=uboot_config_r
vol_size=256KiB
EOF
}

uboot_env_multiubi_mkfs() {
	local mkubifs_args="$1"
	local ubinize_args="$2"

	# Added prompt error message for ubi and ubifs image creation.
	if [ -z "$mkubifs_args" ] || [ -z "$ubinize_args" ]; then
		bbfatal "MKUBIFS_ARGS and UBINIZE_ARGS have to be set, see http://www.linux-mtd.infradead.org/faq/ubifs.html for details"
	fi

	if [ -z "$3" ]; then
		local vname=""
	else
		local vname="_$3"
	fi
	write_ubi_uboot_env_config "${vname}"

	ubinize -o ${IMGDEPLOYDIR}/${IMAGE_U_BOOT_ENV_NAME}${vname}_sdcard.ubi ${ubinize_args} ubinize${vname}-${IMAGE_U_BOOT_ENV_NAME}.cfg

	# Cleanup cfg file
	mv ubinize${vname}-${IMAGE_U_BOOT_ENV_NAME}.cfg ${IMGDEPLOYDIR}/

	# Create own symlinks for 'named' volumes
	if [ -n "$vname" ]; then
		cd ${IMGDEPLOYDIR}
		if [ -e ${IMAGE_U_BOOT_ENV_NAME}${vname}_sdcard.ubifs ]; then
			ln -sf ${IMAGE_U_BOOT_ENV_NAME}${vname}_sdcard.ubifs \
			${IMAGE_LINK_NAME}_u-boot-env-image${vname}_sdcard.ubifs
		fi
		if [ -e ${IMAGE_U_BOOT_ENV_NAME}${vname}_sdcard.ubi ]; then
			ln -sf ${IMAGE_U_BOOT_ENV_NAME}${vname}_sdcard.ubi \
			${IMAGE_LINK_NAME}_u-boot-env-image${vname}_sdcard.ubi
		fi
		cd -
	fi

}


IMAGE_CMD:ubootenvmultiubi () {
	${@' '.join(['%s_%s="%s";' % (arg, name, d.getVar('%s_%s' % (arg, name))) for arg in d.getVar('MULTIUBI_ARGS').split() for name in d.getVar('MULTIUBI_BUILD').split()])}
	# Split MKUBIFS_ARGS_<name> and UBINIZE_ARGS_<name>
	for name in ${MULTIUBI_BUILD}; do
		eval local mkubifs_args=\"\$MKUBIFS_ARGS_${name}\"
		eval local ubinize_args=\"\$UBINIZE_ARGS_${name}\"

		uboot_env_multiubi_mkfs "${mkubifs_args}" "${ubinize_args}" "${name}"
	done
}

do_image_ubootenvmultiubi[depends] += " \
        mtd-utils-native:do_populate_sysroot \
        bc-native:do_populate_sysroot \
        "

