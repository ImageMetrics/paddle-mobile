#!/usr/bin/env sh

build_linux_fn() {
    if [ ! `which protoc` ]; then
        echo "please install the latest protobuf using homebrew"
        return
        # echo "installing protobuf."
        # brew install protobuf
        # if [ ! $? ]; then
        #     echo "protobuf install failed."
        #     return
        # fi
    fi
    if [ ! `which cmake` ]; then
        echo "installing cmake."
        brew install cmake
        if [ ! $? ]; then
            echo "cmake install failed."
            return
        fi
    fi
    PLATFORM="x86"
    MODE="Release"
    CXX_FLAGS="-std=c++11 -DMDL_LINUX"
    LD_FLAGS="-pthread "
    BUILD_DIR=build/release/"${PLATFORM}"
    mkdir -p ${BUILD_DIR}/build
    cp -r test/model ${BUILD_DIR}/build
    cd "${BUILD_DIR}"
    CMAKE="cmake"
    "${CMAKE}" ../../.. \
        -DCMAKE_BUILD_TYPE="${MODE}" \
        -DCMAKE_CXX_FLAGS="${CXX_FLAGS}" \
        -DCMAKE_EXE_LINKER_FLAGS="${LD_FLAGS}" \
        -DIS_MAC=true

    make -j 8
}

build_mac_fn() {
    if [ ! `which brew` ]; then
        echo "building failed! homebrew not found, please install homebrew."
        return
    fi
    if [ ! `which protoc` ]; then
        echo "please install the latest protobuf using homebrew"
        return
        # echo "installing protobuf."
        # brew install protobuf
        # if [ ! $? ]; then
        #     echo "protobuf install failed."
        #     return
        # fi
    fi
    if [ ! `which cmake` ]; then
        echo "installing cmake."
        brew install cmake
        if [ ! $? ]; then
            echo "cmake install failed."
            return
        fi
    fi
    PLATFORM="x86"
    MODE="Release"
    CXX_FLAGS="-std=c++11"
    BUILD_DIR=build/release/"${PLATFORM}"
    mkdir -p ${BUILD_DIR}/build
    cp -r test/model ${BUILD_DIR}/build
    cd "${BUILD_DIR}"
    CMAKE="cmake"
    "${CMAKE}" ../../.. -G Xcode \
        -DCMAKE_BUILD_TYPE="${MODE}" \
        -DCMAKE_CXX_FLAGS="${CXX_FLAGS}" \
        -DIS_MAC=true

    # make -j 8
}

build_android_fn () {
    if [ -z "${NDK_ROOT}" ]; then
        echo "NDK_ROOT not found!"
        exit -1
    fi
    
    # PLATFORM="arm-v7a"
    # PLATFORM="arm-v8a"
    # PLATFORM="x86"
    PLATFORM="x86_64"
    
    if [ "${PLATFORM}" = "arm-v7a" ]; then
        ABI="armeabi-v7a with NEON"
    elif [ "${PLATFORM}" = "arm-v8a" ]; then
        ABI="arm64-v8a"
    elif [ "${PLATFORM}" = "x86" ]; then
        ABI="x86"
    elif [ "${PLATFORM}" = "x86_64" ]; then
        ABI="x86_64"
    else
        echo "unknown platform!"
    fi
    
    MODE="Release"
    ANDROID_PLATFORM_VERSION="android-15"
    
    if [ "${PLATFORM}" = "arm-v7a" ]; then
        CXX_FLAGS="-fomit-frame-pointer -fstrict-aliasing -funswitch-loops -finline-limit=300 -O3 -DNDEBUG -fexceptions -frtti -Wno-psabi -funwind-tables -fsigned-char -no-canonical-prefixes -fdata-sections -ffunction-sections -Wa,--noexecstack -fopenmp -std=c++11 -fPIC -Wall -Wno-sign-compare -Wno-uninitialized -s -fPIE -pie -mtune=arm7 -mfpu=neon -llog"
    else
        CXX_FLAGS="-fomit-frame-pointer -fstrict-aliasing -funswitch-loops -finline-limit=300 -O3 -DNDEBUG -fexceptions -frtti -Wno-psabi -funwind-tables -fsigned-char -no-canonical-prefixes -fdata-sections -ffunction-sections -Wa,--noexecstack -fopenmp -std=c++11 -fPIC -Wall -Wno-sign-compare -Wno-uninitialized -s -fPIE -pie -llog"
    fi
    
    # TOOLCHAIN_FILE="./android-cmake/android.toolchain.cmake"
    TOOLCHAIN_FILE="${NDK_ROOT}/build/cmake/android.toolchain-Oz-lto.cmake"
    # TOOLCHAIN_FILE="${NDK_ROOT}/build/cmake/android.toolchain.cmake"
    
    BUILD_DIR="build/release/16b/${PLATFORM}"

    CMAKE=$(realpath $(find "${NDK_ROOT}/../cmake/" -name "cmake" | grep "bin/cmake"))
    "${CMAKE}" . \
        "-GAndroid Gradle - Unix Makefiles" \
        -B"${BUILD_DIR}" \
        -DANDROID_ABI="${ABI}" \
        -DCMAKE_BUILD_TYPE="${MODE}" \
        -DCMAKE_TOOLCHAIN_FILE="${TOOLCHAIN_FILE}" \
        -DANDROID_PLATFORM="${ANDROID_PLATFORM_VERSION}" \
        -DCMAKE_CXX_FLAGS="${CXX_FLAGS}" \
        -DANDROID="true" \
        -DANDROID_DISABLE_FORMAT_STRING_CHECKS="true" \
        -DANDROID_STL=c++_static

    cp -r test/model ${BUILD_DIR}/build
    cp scripts/deploy_android.sh ${BUILD_DIR}/build
    cd "${BUILD_DIR}"
    make -j 8
}

build_ios_fn () {
    PLATFORM="ios"
    MODE="Release"
    TOOLCHAIN_FILE="./ios-cmake/iOS.toolchain.cmake"
    BUILD_DIR=build/release/"${PLATFORM}"
    mkdir -p "${BUILD_DIR}"

    cmake . -G Xcode \
        -B"${BUILD_DIR}" \
        -DCMAKE_BUILD_TYPE="${MODE}" \
        -DCMAKE_CXX_FLAGS="-std=c++11"
    
    cd "${BUILD_DIR}"
    # make -j 8
}

error_fn () {
    echo "unknown argument"
}

if [ $# = 0 ]; then
    echo "error: target missing!"
    echo "available targets: mac|linux|ios|android"
    echo "sample usage: ./build.sh mac"
else
    if [ $1 = "mac" ]; then
        build_mac_fn
    elif [ $1 = "linux" ]; then
        build_linux_fn
    elif [ $1 = "android" ]; then
        build_android_fn
    elif [ $1 = "ios" ]; then
        build_ios_fn
    else
        error_fn
    fi
fi

