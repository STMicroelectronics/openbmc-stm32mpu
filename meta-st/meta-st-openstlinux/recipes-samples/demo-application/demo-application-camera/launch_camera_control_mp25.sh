#!/bin/sh

cmd() {
    cmd=$1
    echo "$cmd"
    eval $cmd > /dev/null 2>&1
}

is_dcmipp_present() {
    DCMIPP_SENSOR="NOTFOUND"
    # on disco board ov5640 camera can be present on csi connector
    for video in $(find /sys/class/video4linux -name "video*" -type l);
    do
        if [ "$(cat $video/name)" = "dcmipp_main_capture" ]; then
            cd $video/device/
            mediadev=/dev/$(ls -d media*)
            cd -
            for sub in $(find /sys/class/video4linux -name "v4l-subdev*" -type l);
            do
                subdev_name=$(tr -d '\0' < $sub/name | awk '{print $1}')
                #HFR FIXME get rid of driver name
                if [ "$subdev_name" = "gc2145" ] || [ "$subdev_name" = "ov5640" ] || [ "$subdev_name" = "imx335" ]; then
                    DCMIPP_SENSOR=$subdev_name
                    V4L_DEVICE="device=/dev/$(basename $video)"
                    sensorsubdev="$(tr -d '\0' < $sub/name)"
                    sensordev=$(media-ctl -d $mediadev -p -e "$sensorsubdev" | grep "node name" | awk -F\name '{print $2}')
                    #interface is connected to input of dcmipp_input (":1 [ENABLED" with media-ctl -p)
                    interfacesubdev=$(media-ctl -d $mediadev -p -e "dcmipp_input" | grep ":1 \[ENABLED" | awk -F\" '{print $2}')
                    echo "mediadev="$mediadev
                    echo "sensorsubdev=\""$sensorsubdev\"
                    echo "sensordev="$sensordev
                    echo "interfacesubdev="$interfacesubdev

                    return
                fi
            done
        fi
    done
}

get_webcam_device() {
    found="NOTFOUND"
    for video in $(find /sys/class/video4linux -name "video*" -type l | sort);
    do
        if [ "$(cat $video/name)" = "dcmipp_main_capture" ]; then
            found="FOUND"
        else
            V4L_DEVICE="device=/dev/$(basename $video)"
            break;
        fi
    done
}

# ------------------------------
#         main
# ------------------------------

# graphic backend detection
if [ -f /etc/default/weston ] && $(grep "^OPTARGS" /etc/default/weston | grep -q "use-pixman" ) ;
then
	echo "Without GPU"
	ADDONS="videoconvert ! queue !"
else
	echo "With GPU"
	ADDONS=""
fi

WIDTH=640
HEIGHT=480
FPS=30
FMT=RGB16
#HFR FIXME
displaybuscode=RGB565_2X8_LE

# camera detection
is_dcmipp_present
if [ "$DCMIPP_SENSOR" != "NOTFOUND" ]; then
    #Use sensor in raw-bayer format
    sensorbuscode=`v4l2-ctl --list-subdev-mbus-codes -d $sensordev | grep SRGGB | awk -FMEDIA_BUS_FMT_ '{print $2}'`
    echo "sensorbuscode="$sensorbuscode

    SENSORWIDTH=0
    SENSORHEIGHT=0
    if [ "$DCMIPP_SENSOR" = "ov5640" ]; then
        #OV5640 only support 720p with raw-bayer format
        SENSORWIDTH=1280
        SENSORHEIGHT=720
        #OV5640 claims to support all raw bayer combinations but always output SBGGR8_1X8...
        sensorbuscode=SBGGR8_1X8
    elif [ "$DCMIPP_SENSOR" = "imx335" ]; then
        main_postproc=`media-ctl -d $mediadev -e dcmipp_main_postproc`
        #Enable gamma correction
        v4l2-ctl -d $main_postproc -c gamma_correction=1
        #Do exposure correction continuously in background
        sleep 3  && while : ; do /usr/local/demo/bin/dcmipp-isp-ctrl -i0 -g > /dev/null ; done &
    fi

    #Let sensor return its prefered resolution & format
    media-ctl -d $mediadev --set-v4l2 "'$sensorsubdev':0[fmt:$sensorbuscode/${SENSORWIDTH}x${SENSORHEIGHT}@1/${FPS} field:none]" > /dev/null 2>&1
    sensorfmt=`media-ctl -d $mediadev --get-v4l2 "'$sensorsubdev':0" | awk -F"fmt:" '{print $2}' | awk -F" " '{print $1}'`
    SENSORWIDTH=`echo $sensorfmt | awk -F"/" '{print $2}' | awk -F"x" '{print $1}'`
    SENSORHEIGHT=`echo $sensorfmt | awk -F"/" '{print $2}' | awk -F"x" '{print $2}' | awk -F" " '{print $1}' | awk -F"@" '{print $1}'`
    echo "SENSORWIDTH="$SENSORWIDTH
    echo "SENSORHEIGHT="$SENSORHEIGHT

    #Use main pipe for debayering, scaling and color conversion
echo "Mediacontroller graph:"
    cmd "  media-ctl -d $mediadev --set-v4l2 \"'$sensorsubdev':0[fmt:$sensorbuscode/${SENSORWIDTH}x${SENSORHEIGHT}]\""
    cmd "  media-ctl -d $mediadev --set-v4l2 \"'$interfacesubdev':1[fmt:$sensorbuscode/${SENSORWIDTH}x${SENSORHEIGHT}]\""
    cmd "  media-ctl -d $mediadev --set-v4l2 \"'dcmipp_input':2[fmt:$sensorbuscode/${SENSORWIDTH}x${SENSORHEIGHT}]\""
    cmd "  media-ctl -d $mediadev --set-v4l2 \"'dcmipp_main_isp':1[fmt:RGB888_1X24/${SENSORWIDTH}x${SENSORHEIGHT} field:none]\""
    cmd "  media-ctl -d $mediadev --set-v4l2 \"'dcmipp_main_postproc':0[compose:(0,0)/${WIDTH}x${HEIGHT}]\""
    cmd "  media-ctl -d $mediadev --set-v4l2 \"'dcmipp_main_postproc':1[fmt:$displaybuscode/${WIDTH}x${HEIGHT}]\""
echo ""

    V4L2_CAPS="video/x-raw, format=$FMT, width=$WIDTH, height=$HEIGHT"
    V4L_OPT=""
else
    get_webcam_device
    # suppose we have a webcam
    V4L2_CAPS="video/x-raw, width=$WIDTH, height=$HEIGHT"
    V4L_OPT="io-mode=4"
    v4l2-ctl --set-parm=20
fi

# Detect size of screen
SCREEN_WIDTH=$(wayland-info | grep logical_width | sed -r "s/logical_width: ([0-9]+),.*/\1/")
SCREEN_HEIGHT=$(wayland-info | grep logical_width | sed -r "s/.*logical_height: ([0-9]+).*/\1/")
echo "SCREEN_WIDTH="$SCREEN_WIDTH
echo "SCREEN_HEIGHT="$SCREEN_HEIGHT
