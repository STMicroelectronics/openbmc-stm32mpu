#!/bin/sh

if pidof -qx -o $$ $(basename "${0}") ; then
  echo "Process $(basename "${0}") is already running"
  exit 1
else
  if (/usr/local/demo/application/camera/bin/check_camera_preview@suffix_for_file@.sh); then
    /usr/local/demo/application/camera/bin/launch_camera_preview@suffix_for_file@.sh
    exit ${?}
  else
    exit 2
  fi
fi

