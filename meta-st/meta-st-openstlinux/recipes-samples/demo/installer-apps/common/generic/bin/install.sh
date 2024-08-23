#!/usr/bin/env bash

# Check if we have at least one argument
[ $# -lt 1 ] && exit 1

source /usr/local/demo/common/libsh.sh

trap cleanup EXIT

# we do not need to perform apt-get update as the demo
# is provided by OSTL and the apt-get update has been
# done by the X-Linux package installer

# Install the given packages
apt_get_install $@
