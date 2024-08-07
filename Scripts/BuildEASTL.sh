#!/bin/bash

EASTL_FOLDER=external/EASTL
BUILD_FOLDER=build
RELEASE_DIR=${BUILD_FOLDER}/release
DEBUG_DIR=${BUILD_FOLDER}/debug

# NOTE: Submodule update is not recursive as a recursive update is horrendously slow
echo "Updating EASTL"
git submodule update --init

pushd ${EASTL_FOLDER}

echo "Updating EASTL submodules"
git submodule update --init

echo "Clearing build folder ${EASTL_FOLDER}/${BUILD_FOLDER}"
rm -rf ${BUILD_FOLDER}

mkdir -p ${DEBUG_DIR} ${RELEASE_DIR}

cmake_args="-DEASTL_BUILD_TESTS:BOOL=OFF -DEASTL_BUILD_BENCHMARK:BOOL=OFF"

build() {
  local build_dir=$1
  local build_type=$2
  cmake -S . -B ${build_dir} ${cmake_args}
  echo "Building ${build_type}"
  cmake --build ${build_dir} --config ${build_type} --parallel 32
}

build ${RELEASE_DIR} Release
build ${DEBUG_DIR} Debug

popd