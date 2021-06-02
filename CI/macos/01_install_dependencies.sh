#!/bin/bash

##############################################################################
# macOS dependency management function
##############################################################################
#
# This script file can be included in build scripts for macOS or run directly
#
##############################################################################

# Halt on errors
set -eE

install_obs-deps() {
    status "Setup for pre-built macOS OBS dependencies v${1}"
    ensure_dir "${DEPS_BUILD_DIR}"
    step "Download..."
    ${CURLCMD:-curl} https://github.com/obsproject/obs-deps/releases/download/${1}/macos-deps-${1}.tar.gz
    step "Unpack..."
    /usr/bin/tar -xf "./macos-deps-${1}.tar.gz" -C /tmp
}

install_qt-deps() {
    status "Setup for pre-built dependency Qt v${1}"
    ensure_dir "${DEPS_BUILD_DIR}"
    step "Download..."
    ${CURLCMD:-curl} https://github.com/obsproject/obs-deps/releases/download/${2}/macos-qt-${1}-${2}.tar.gz
    step "Unpack..."
    /usr/bin/tar -xf ./macos-qt-${1}-${2}.tar.gz -C /tmp
    /usr/bin/xattr -r -d com.apple.quarantine /tmp/obsdeps
}

install_vlc() {
    status "Setup for dependency VLC v${1}"
    ensure_dir "${DEPS_BUILD_DIR}"

    if [ "${CI}" -a "${RESTORED_VLC}" ]; then
        _SKIP=TRUE
    elif [ -d "${DEPS_BUILD_DIR}/vlc-${1}" -a -f "${DEPS_BUILD_DIR}/vlc-${1}/include/vlc/vlc.h" ]; then
        _SKIP=TRUE
    fi

    if [ ! -n "${_SKIP}" ]; then
        step "Download..."
        ${CURLCMD:-curl} https://downloads.videolan.org/vlc/${1}/vlc-${1}.tar.xz
        step "Unpack..."
        /usr/bin/tar -xf vlc-${1}.tar.xz
    else
        step "Found existing VLC..."
    fi
}

install_sparkle() {
    status "Setup for dependency Sparkle v${1}"
    ensure_dir "${DEPS_BUILD_DIR}"

    if [ "${CI}" -a "${RESTORED_SPARKLE}" ]; then
        _SKIP=TRUE
    elif [ -d "${DEPS_BUILD_DIR}/sparkle/Sparkle.framework" -a -f "${DEPS_BUILD_DIR}/sparkle/Sparkle.framework/Sparkle" ]; then
        _SKIP=TRUE
    fi

    if [ -z "${_SKIP}" ]; then
        step "Download..."
        ${CURLCMD:-curl} https://github.com/sparkle-project/Sparkle/releases/download/${1}/Sparkle-${1}.tar.xz
        step "Unpack..."
        ensure_dir "${DEPS_BUILD_DIR}/sparkle"
        /usr/bin/tar -xf ../Sparkle-${1}.tar.xz
    else
        step "Found existing Sparkle Framework..."
    fi
}

install_cef() {
    status "Setup for dependency CEF v${1}"
    ensure_dir "${DEPS_BUILD_DIR}"

    if [ "${CI}" -a "${RESTORED_CEF}" ]; then
        _SKIP=TRUE
    elif [ -d "${DEPS_BUILD_DIR}/cef_binary_${1}_macos_${ARCH:-x86_64}" -a -f "${DEPS_BUILD_DIR}/cef_binary_${1}_macos_${ARCH:-x86_64}/build/libcef_dll_wrapper/libcef_dll_wrapper.a" ]; then
        _SKIP=TRUE
    fi

    if [ -z "${_SKIP}" ]; then
        step "Download..."
        ${CURLCMD:-curl} https://cdn-fastly.obsproject.com/downloads/cef_binary_${1}_macos_${ARCH:-x86_64}.tar.xz
        step "Unpack..."
        /usr/bin/tar -xf cef_binary_${1}_macos_${ARCH:-x86_64}.tar.xz
        cd cef_binary_${1}_macos_${ARCH:-x86_64}
        step "Fix tests..."

        /usr/bin/sed -i '.orig' '/add_subdirectory(tests\/ceftests)/d' ./CMakeLists.txt
        /usr/bin/sed -E -i '' 's/"10.(9|10)"/"'${MIN_MACOS_VERSION:-${CI_MIN_MACOS_VERSION}}'"/' ./cmake/cef_variables.cmake

        step "Run CMake..."
        check_ccache
        cmake ${CCACHE_OPTIONS} ${QUIET:+-Wno-deprecated -Wno-dev --log-level=ERROR} \
            -S . -B build \
            -G Ninja \
            -DPROJECT_ARCH=${ARCH:-x86_64} \
            -DCEF_COMPILER_FLAGS="-Wno-deprecated-copy" \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_CXX_FLAGS="-std=c++11 -stdlib=libc++ -Wno-deprecated-declarations -Wno-unknown-warning-option" \
            -DCMAKE_EXE_LINKER_FLAGS="-std=c++11 -stdlib=libc++" \
            -DCMAKE_OSX_DEPLOYMENT_TARGET=${MIN_MACOS_VERSION:-${CI_MIN_MACOS_VERSION}}

        step "Build CEF v${1}..."
        cmake --build build
        mkdir -p build/libcef_dll
    else
        step "Found existing CEF Framework and loader library..."
    fi
}

install_dependencies() {
    status "Installation of Homebrew dependencies"
    trap "caught_error 'install_dependencies'" ERR

    BUILD_DEPS=(
        "obs-deps ${MACOS_DEPS_VERSION:-${CI_DEPS_VERSION}}"
        "qt-deps ${QT_VERSION:-${CI_QT_VERSION}} ${MACOS_DEPS_VERSION:-${CI_DEPS_VERSION}}"
        "cef ${MACOS_CEF_BUILD_VERSION:-${CI_MACOS_CEF_VERSION}}"
        "vlc ${VLC_VERSION:-${CI_VLC_VERSION}}"
        "sparkle ${SPARKLE_VERSION:-${CI_SPARKLE_VERSION}}"
    )

    install_homebrew_deps

    for DEPENDENCY in "${BUILD_DEPS[@]}"; do
        set -- ${DEPENDENCY}
        trap "caught_error ${DEPENDENCY}" ERR
        FUNC_NAME="install_${1}"
        ${FUNC_NAME} ${2} ${3}
    done
}

install-dependencies-standalone() {
    CHECKOUT_DIR="$(/usr/bin/git rev-parse --show-toplevel)"
    PRODUCT_NAME="OBS-Studio"
    DEPS_BUILD_DIR="${CHECKOUT_DIR}/../obs-build-dependencies"
    source ${CHECKOUT_DIR}/CI/include/build_support.sh
    source ${CHECKOUT_DIR}/CI/include/build_support_macos.sh

    status "Setup of OBS build dependencies"
    step "Check macOS version..."
    check_macos_version
    install_dependencies

    info "All done!"
}

print_usage() {
    echo -e "Usage: ${0}\n" \
            "-h, --help                     : Print this help\n" \
            "-q, --quiet                    : Suppress most build process output\n" \
            "-v, --verbose                  : Enable more verbose build process output\n" \
            "-a, --architecture             : Specify build architecture (default: x86_64, alternative: arm64)\n"
    exit 0
}

install-dependencies-main() {
    if [ -z "${_RUN_OBS_BUILD_SCRIPT}" ]; then
        while true; do
            case "${1}" in
                -h | --help ) print_usage ;;
                -q | --quiet ) export QUIET=TRUE; shift ;;
                -v | --verbose ) export VERBOSE=TRUE; shift ;;
                -a | --architecture ) ARCH="${2}"; shift 2 ;;
                -- ) shift; break ;;
                * ) break ;;
            esac
        done

        install-dependencies-standalone
    fi
}

install-dependencies-main $*
