#!/bin/bash

##############################################################################
# Linux support functions
##############################################################################
#
# This script file can be included in build scripts for Linux.
#
##############################################################################

# Setup build environment

CI_LINUX_CEF_VERSION=$(/bin/cat "${CI_WORKFLOW}" | /usr/bin/sed -En "s/[ ]+CEF_BUILD_VERSION_LINUX: '([0-9]+)'/\1/p")

if [ "${TERM-}" -a -z "${CI}" ]; then
    COLOR_RED=$(/usr/bin/tput setaf 1)
    COLOR_GREEN=$(/usr/bin/tput setaf 2)
    COLOR_BLUE=$(/usr/bin/tput setaf 4)
    COLOR_ORANGE=$(/usr/bin/tput setaf 3)
    COLOR_RESET=$(/usr/bin/tput sgr0)
else
    COLOR_RED=""
    COLOR_GREEN=""
    COLOR_BLUE=""
    COLOR_ORANGE=""
    COLOR_RESET=""
fi

if [ "${CI}" -o "${QUIET}" ]; then
    export CURLCMD="curl --silent --show-error --location -O"
else
    export CURLCMD="curl --progress-bar --location --continue-at - -O"
fi
