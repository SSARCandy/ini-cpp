#!/bin/bash

PROGNAME=${0##*/}

CURDIR="$( cd "$(dirname "$0")" ; pwd -P )"
BUILD_TYPE="Release"
BUILD_DIR="build"
TEST="False"

if which nproc; then
    # Linux
    CORES="$(nproc --all)"
elif which sysctl; then
    # MacOS
    CORES="$(sysctl -n hw.logicalcpu)"
else
    CORES=2
fi
JOBS=$(( (CORES + 1) / 2 ))
usage() {

cat <<EOF

  Usage: $PROGNAME [options]

  Options:

    -h, --help        Display this help and exit
    -c, --clean       Clean build
    -d, --debug       Build with debug mode
    -j, --jobs        Use N cores to build
    -t, --tetst       Build Debug and run tests

EOF
}

clean() {
    rm -rf "$BUILD_DIR"
}

test() {
    cd ./build/test;
    ./all_test
    cd $CURDIR;

    lcov \
        --capture \
        --directory build/test/ \
        --output-file coverage.info \
        --test-name coverageHtml > /dev/null

    lcov  -o coverage.info --extract coverage.info "${CURDIR}/ini/*" > /dev/null
    genhtml -o .coverage coverage.info
}


while (( "$#" )); do
    case "$1" in
        -h|--help)
            usage
            exit 0
            ;;
        -j|--jobs)
            JOBS="$2"
            shift 2
            ;;
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        -t|--test)
            BUILD_TYPE="Debug"
            TEST="True"
            shift
            ;;
        -c|--clean)
            clean
            shift
            ;;
        -*|--*=)
            echo "Invalid arguments"
            exit 1
            ;;
        *)
            break
            ;;
    esac
done

BUILD_OPTIONS="-DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"
if cmake .. ${BUILD_OPTIONS}; then
     if ! make -j "${JOBS}"; then
         exit 1
     fi
     cd "$CURDIR"
    
    if [ "${TEST}" == "True" ]; then
        test
        cd "$CURDIR"
    fi
else
    cd "$CURDIR"
    exit 1
fi
