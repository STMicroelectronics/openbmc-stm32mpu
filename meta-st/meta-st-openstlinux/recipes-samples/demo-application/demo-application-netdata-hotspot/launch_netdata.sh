#!/bin/sh

if pidof -qx -o $$ $(basename "${0}") ; then
  echo "Process $(basename "${0}") is already running"
  exit 1
else
  python3 /usr/local/demo/application/netdata/netdata.py
  exit ${?}
fi

