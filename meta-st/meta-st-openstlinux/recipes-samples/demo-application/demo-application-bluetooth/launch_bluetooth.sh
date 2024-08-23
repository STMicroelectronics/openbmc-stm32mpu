#!/bin/sh

if pidof -qx -o $$ $(basename "${0}") ; then
  echo "Process $(basename "${0}") is already running"
  exit 1
else
  if (/usr/local/demo/application/bluetooth/bin/check_ble.sh); then
    /usr/local/demo/application/bluetooth/bluetooth_audio.py
    exit ${?}
  else
    exit 2
  fi
fi

