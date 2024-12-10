#!/bin/sh -
/sbin/ip link show wlan0 | head -n 1 | grep -q DOWN && echo -n DOWN
/sbin/ip link show wlan0 | head -n 1 | grep -q UP && echo -n UP
