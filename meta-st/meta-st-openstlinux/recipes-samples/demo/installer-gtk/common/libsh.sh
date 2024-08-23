#!/usr/bin/env bash
#
# This file should be sourced; an error is raised if not
#

# Single inclusion
[ -z "${_libsh_included-}" ] || return 0
_libsh_included=1

# Create tmp directory
_libsh_tmpdir=$(mktemp -d)

# Clean-up function
function cleanup {
    [ -n "${_libsh_tmpdir:-}" ] && [ -d "${_libsh_tmpdir}" ] && rm -rf "${_libsh_tmpdir}"
    trap - EXIT
}

# Get the tmp directory
function get_tmpdir() {
    echo "${_libsh_tmpdir}"
}

# Update apt cache
function apt_get_update() {
    sudo -E apt-get update || exit 1
}

# Install new packages
function apt_get_install() {
    sudo -E apt-get install --assume-yes $@ || exit 1
}

# Remove packages and clean archives
function apt_get_remove() {
    sudo -E apt-get remove --assume-yes $@  || exit 1
    sudo -E apt-get autoremove --assume-yes || exit 1
}

# Download file from URL
function download_file_from_url() {
    local in=${1:?}
    local out=${2:?}
    curl "${in}" --output "${out}" || exit 1
}

# Try to execute a `return` statement, but do it in a sub-shell
# (hence disable corresponding shellcheck message) and catch the results.
# If this script is not sourced, that will raise an error.
# shellcheck disable=SC2091
$(return >/dev/null 2>&1)

# What exit code did that give?
# shellcheck disable=SC2181
[ "$?" -eq "0" ] || error "This script should be sourced. Aborting..."
