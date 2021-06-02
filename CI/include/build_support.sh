#!/bin/bash

##############################################################################
# Unix support functions
##############################################################################
#
# This script file can be included in build scripts for UNIX-compatible
# shells to compose build scripts.
#
##############################################################################

## DEFINE UTILITIES ##

if [ -z "${QUIET}" ]; then
    status() {
        echo -e "${COLOR_BLUE}[${PRODUCT_NAME}] ${1}${COLOR_RESET}"
    }

    step() {
        echo -e "${COLOR_GREEN}  + ${1}${COLOR_RESET}"
    }

    info() {
        echo -e "${COLOR_ORANGE}  + ${1}${COLOR_RESET}"
    }

    error() {
        echo -e "${COLOR_RED}  + ${1}${COLOR_RESET}"
    }
else
    status() {
        :
    }

    step() {
        :
    }

    info() {
        :
    }

    error() {
        echo -e "${COLOR_RED}  + ${1}${COLOR_RESET}"
    }
fi

exists() {
  /usr/bin/command -v "$1" >/dev/null 2>&1
}

ensure_dir() {
    [ -n "${1}" ] && /bin/mkdir -p "${1}" && builtin cd "${1}"
}

cleanup() {
    :
}

caught_error() {
    error "ERROR during build step: ${1}"
    cleanup
    exit 1
}

# Setup build environment

BUILD_DIR="${BUILD_DIR:-build}"
BUILD_CONFIG=${BUILD_CONFIG:-RelWithDebInfo}
CI_WORKFLOW="${CHECKOUT_DIR}/.github/workflows/main.yml"
CURRENT_ARCH=$(uname -m)

## Utility functions ##

check_ccache() {
    if ccache -s >/dev/null 2>&1; then
        info "CCache available"
        export CCACHE_OPTIONS="-DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_C_COMPILER_LAUNCHER=ccache"

        if [ "${CI}" ]; then
            ccache --set-config=cache_dir=${GITHUB_WORKSPACE}/.ccache
            ccache --set-config=max_size=${CCACHE_SIZE:-500M}
            ccache --set-config=compression=true
            ccache -z
        fi
    else
        info "CCache not available"
    fi
}
