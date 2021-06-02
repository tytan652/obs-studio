#!/bin/bash

##############################################################################
# macOS support functions
##############################################################################
#
# This script file can be included in build scripts for macOS.
#
##############################################################################

# Setup build environment

CI_DEPS_VERSION=$(/bin/cat "${CI_WORKFLOW}" | /usr/bin/sed -En "s/[ ]+DEPS_VERSION_MAC: '([0-9\-]+)'/\1/p")
CI_VLC_VERSION=$(/bin/cat "${CI_WORKFLOW}" | /usr/bin/sed -En "s/[ ]+VLC_VERSION_MAC: '([0-9\.]+)'/\1/p")
CI_SPARKLE_VERSION=$(/bin/cat "${CI_WORKFLOW}" | /usr/bin/sed -En "s/[ ]+SPARKLE_VERSION: '([0-9\.]+)'/\1/p")
CI_QT_VERSION=$(/bin/cat "${CI_WORKFLOW}" | /usr/bin/sed -En "s/[ ]+QT_VERSION_MAC: '([0-9\.]+)'/\1/p" | /usr/bin/head -1)
CI_MIN_MACOS_VERSION=$(/bin/cat "${CI_WORKFLOW}" | /usr/bin/sed -En "s/[ ]+MIN_MACOS_VERSION: '([0-9\.]+)'/\1/p")
CI_MACOS_CEF_VERSION=$(/bin/cat "${CI_WORKFLOW}" | /usr/bin/sed -En "s/[ ]+CEF_BUILD_VERSION_MAC: '([0-9]+)'/\1/p")

MACOS_VERSION="$(/usr/bin/sw_vers -productVersion)"
MACOS_MAJOR="$(echo ${MACOS_VERSION} | /usr/bin/cut -d '.' -f 1)"
MACOS_MINOR="$(echo ${MACOS_VERSION} | /usr/bin/cut -d '.' -f 2)"

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

## CHECK AND INSTALL DEPENDENCIES ##
check_macos_version() {
    MIN_VERSION=${MIN_MACOS_VERSION:-${CI_MIN_MACOS_VERSION}}
    MIN_MAJOR=$(echo ${MIN_VERSION} | /usr/bin/cut -d '.' -f 1)
    MIN_MINOR=$(echo ${MIN_VERSION} | /usr/bin/cut -d '.' -f 2)

    if [ "${MACOS_MAJOR}" -lt "11" -a "${MACOS_MINOR}" -lt "${MIN_MINOR}" ]; then
        error "ERROR: Minimum required macOS version is ${MIN_VERSION}, but running on ${MACOS_VERSION}"
    fi

    if [ "${MACOS_MAJOR}" -ge "11" ]; then
        export CODESIGN_LINKER="ON"
    fi
}

install_homebrew_deps() {
    if ! exists brew; then
        error "Homebrew not found - please install homebrew (https://brew.sh)"
        exit 1
    fi

    brew bundle --file "${CHECKOUT_DIR}/CI/include/Brewfile" ${QUIET:+--quiet}

    check_curl
}

check_curl() {
    if [ "${MACOS_MAJOR}" -lt "11" -a "${MACOS_MINOR}" -lt "15" ]; then
        if [ ! -d /usr/local/opt/curl ]; then
            step "Install Homebrew curl..."
            brew install curl
        fi

        CURLCMD="/usr/local/opt/curl/bin/curl"
    else
        CURLCMD="curl"
    fi

    if [ "${CI}" -o "${QUIET}" ]; then
        export CURLCMD="${CURLCMD} --silent --show-error --location -O"
    else
        export CURLCMD="${CURLCMD} --progress-bar --location --continue-at - -O"
    fi
}

## SET UP CODE SIGNING AND NOTARIZATION CREDENTIALS ##
##############################################################################
# Apple Developer Identity needed:
#
#    + Signing the code requires a developer identity in the system's keychain
#    + codesign will look up and find the identity automatically
#
##############################################################################
read_codesign_ident() {
    if [ -z "${CODESIGN_IDENT}" ]; then
        step "Set up code signing..."
        read -p "${COLOR_ORANGE}  + Apple developer identity: ${COLOR_RESET}" CODESIGN_IDENT
    fi
}

##############################################################################
# Apple Developer credentials necessary:
#
#   + Signing for distribution and notarization require an active Apple
#     Developer membership
#   + An Apple Development identity is needed for code signing
#     (i.e. 'Apple Development: YOUR APPLE ID (PROVIDER)')
#   + Your Apple developer ID is needed for notarization
#   + An app-specific password is necessary for notarization from CLI
#   + This password will be stored in your macOS keychain under the identifier
#     'OBS-Codesign-Password' with access Apple's 'altool' only.
##############################################################################

read_codesign_pass() {
    step "Set up notarization..."

    if [ -z "${CODESIGN_IDENT_USER}" ]; then
        read -p "${COLOR_ORANGE}  + Apple account id: ${COLOR_RESET}" CODESIGN_IDENT_USER
    fi

    if [ -z "${CODESIGN_IDENT_PASS}" ]; then
        CODESIGN_IDENT_PASS=$(stty -echo; read -p "${COLOR_ORANGE}  + Apple developer password: ${COLOR_RESET}" secret; stty echo; echo $secret)
        echo ""
    fi

    step "Update notarization keychain..."

    echo -n "${COLOR_ORANGE}"
    /usr/bin/xcrun altool --store-password-in-keychain-item "OBS-Codesign-Password" -u "${CODESIGN_IDENT_USER}" -p "${CODESIGN_IDENT_PASS}"
    echo -n "${COLOR_RESET}"
    CODESIGN_IDENT_SHORT=$(echo "${CODESIGN_IDENT}" | /usr/bin/sed -En "s/.+\((.+)\)/\1/p")
}
