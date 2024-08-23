#!/usr/bin/env bash

source /usr/local/demo/common/libsh.sh

trap cleanup EXIT

# Update the Gtk demo installer / uninstaller to get the last entries
apt_get_update
apt_get_install installer-gtk installer-apps
