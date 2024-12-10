RDEPENDS:${PN} += "\
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'demo-launcher', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'demo-gtk', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'installer-gtk', '', d)} \
    \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'demo-application-netdata-hotspot', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'demo-application-camera', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'demo-application-video', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'demo-application-3d-cube', '', d)} \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', 'demo-application-bluetooth', '', d)} \
    ${@bb.utils.contains('COMBINED_FEATURES', 'splashscreen', 'psplash-drm-extra', '', d)} \
    "

AI_DEMO_APPLICATION = "${@bb.utils.contains('MACHINE_FEATURES', 'm4copro', 'ai-hand-char-reco-launcher', '', d)} "
RDEPENDS:${PN}:append:stm32mpcommon = " \
    ${@bb.utils.contains('DISTRO_FEATURES', 'wayland', '${AI_DEMO_APPLICATION}', '', d)} \
    "

