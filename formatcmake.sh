#!/usr/bin/env bash

set -o errexit
set -o pipefail
set -o nounset

# Runs the cmake-format in parallel on the code base.

# Get CPU count
OS=$(uname)
NPROC=1
if [[ ${OS} = "Linux" ]] ; then
    NPROC=$(nproc)
elif [[ ${OS} = "Darwin" ]] ; then
    NPROC=$(sysctl -n hw.physicalcpu)
fi

# Discover cmake-format
if type cmake-format 2> /dev/null ; then
    CMAKE_FORMAT=cmake-format
else
    echo "No appropriate cmake-format found"
    exit 1
fi

find . -type d \( \
    -path ./\*build -o \
    -path ./plugins/decklink/\*/decklink-sdk -o \
    -path ./plugins/mac-syphon/syphon-framework -o \
    -path ./plugins/obs-outputs/ftl-sdk \
\) -prune -false -type f -o \
    -name 'CMakeLists.txt' \
 | xargs -I{} -P ${NPROC} ${CMAKE_FORMAT} {} -o {}
