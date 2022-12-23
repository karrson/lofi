#!/bin/bash
if (( $EUID != 0 )); then
	sudo rm -rf configure.mak
	sudo dos2unix configure
	sudo dos2unix ./tools/mkalltypes.sed
	./configure
	exit 0
fi
echo "Don't run as root."
exit 1