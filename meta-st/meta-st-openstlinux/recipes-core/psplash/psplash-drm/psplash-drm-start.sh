#!/bin/sh -

# List of Splash screen available
SPLASH_BG_LANDSCAPE_INDUS_480_272="ST30739_splash-480x272.png"
SPLASH_BG_LANDSCAPE_INDUS_480_800="ST30739_splash-800x480.png"
SPLASH_BG_LANDSCAPE_INDUS_800_480="ST30739_splash-800x480.png"
SPLASH_BG_LANDSCAPE_INDUS_1280_720="ST30739_splash-1280x720.png"

SPLASH_BG_LANDSCAPE_TSN_1024_600="ST30739_splash-1024x600.png"
SPLASH_BG_LANDSCAPE_TSN_1920_1080="ST30739_splash-1920x1080.png"
SPLASH_BG_PORTRAIT_TSN_720_1280="ST30739_splash-720x1280.png"

DEFAULT_SPLASH=$SPLASH_BG_LANDSCAPE_INDUS_480_800
OPT=""
if [ -d /proc/device-tree/ ]; then
	echo "[DEBUG]: test compatible"
	# stm32mp13: 480x272 screen (landscape)
	if $(cat /proc/device-tree/compatible | grep -q "stm32mp13")
	then
		DEFAULT_SPLASH=$SPLASH_BG_LANDSCAPE_INDUS_480_272
		echo "[DEBUG]: compatible mp13"
	fi
	# stm32mp15: 480x800 or 1280x720 (landscape) + HDMI 1280x720
	if  $(cat /proc/device-tree/compatible | grep -q "stm32mp15")
	then
		DEFAULT_SPLASH=$SPLASH_BG_LANDSCAPE_INDUS_800_480
		echo "[DEBUG]: compatible mp15"
		hdmi_status=$(cat /sys/class/drm/card0-HDMI-A-1/status)
		if [ "$hdmi_status" = "connected" ]; then
			DEFAULT_SPLASH=$SPLASH_BG_LANDSCAPE_INDUS_1280_720
		        OPT="$OPT --force-hdmi"
			echo "[DEBUG]: compatible mp15 with hdmi connected"
		fi
	fi
	# stm32mp25: 1024x600 or 1920x1080 (landscape) + HDMI 1280x720 or 1920x1080
	if  $(cat /proc/device-tree/compatible | grep -q "stm32mp25")
	then
		DEFAULT_SPLASH=$SPLASH_BG_LANDSCAPE_TSN_1024_600
		hdmi_status=$(cat /sys/class/drm/card0-HDMI-A-1/status)
		echo "[DEBUG]: compatible mp25"

		OPT="--force-no-hdmi"
		if [ -e  /proc/device-tree/panel-lvds ]; then
			panel_lvds=$(cat /proc/device-tree/panel-lvds/compatible)
			if $(cat /proc/device-tree/panel-lvds/compatible | grep -q "etml0700z8dh" )
			then
				DEFAULT_SPLASH=$SPLASH_BG_LANDSCAPE_TSN_1920_1080
			else
				DEFAULT_SPLASH=$SPLASH_BG_LANDSCAPE_TSN_1024_600
			fi
		fi
		if [ "$hdmi_status" = "connected" ]; then
			lvds_status=$(cat /proc/device-tree/panel-lvds/status)
			if [ "$lvds_status" = "disabled" ]; then
				OPT=" --maxsize=1920 --force-hdmi"
				DEFAULT_SPLASH=$SPLASH_BG_LANDSCAPE_TSN_1920_1080
				echo "[DEBUG]: compatible mp25 with hdmi connected"
			fi
		fi
	fi
fi
echo "/usr/bin/psplash-drm -w $OPT --background 03234b --filename=/usr/share/splashscreen/$DEFAULT_SPLASH"
/usr/bin/psplash-drm -w $OPT --background 03234b --filename=/usr/share/splashscreen/$DEFAULT_SPLASH
