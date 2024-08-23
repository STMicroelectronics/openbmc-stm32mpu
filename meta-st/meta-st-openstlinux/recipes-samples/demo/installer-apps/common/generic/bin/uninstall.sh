#!/usr/bin/env bash

# Check if we have at least one argument
[ $# -lt 1 ] && exit 1

source /usr/local/demo/common/libsh.sh

trap cleanup EXIT

# Uninstall the given packages
apt_get_remove $@
