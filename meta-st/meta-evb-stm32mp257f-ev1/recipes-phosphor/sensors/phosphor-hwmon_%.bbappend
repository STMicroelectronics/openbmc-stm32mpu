inherit obmc-phosphor-utils

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

CHIPS = "\
    thermal-sensor@44070000 \
    "

ITEMS_FMT = "soc@0/{0}.conf"

ITEMS = "${@compose_list(d, 'ITEMS_FMT', 'CHIPS')}"

ENVS = "obmc/hwmon/{0}"
SYSTEMD_ENVIRONMENT_FILE:${PN}:append := " ${@compose_list(d, 'ENVS', 'ITEMS')}"

EXTRA_OEMESON:append = " -Dnegative-errno-on-fail=true"
