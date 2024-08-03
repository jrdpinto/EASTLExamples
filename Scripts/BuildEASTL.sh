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

mkdir -p ${DEBUG_DIR}
mkdir -p ${RELEASE_DIR}

cmake . -DEASTL_BUILD_TESTS:BOOL=OFF -DEASTL_BUILD_BENCHMARK:BOOL=OFF
cmake -S . -B ${RELEASE_DIR}
echo "Building release"
cmake --build ${RELEASE_DIR} --config Release -- -j 32 
cmake -S . -B ${DEBUG_DIR}
echo "Building debug"
cmake --build ${DEBUG_DIR} --config Debug -- -j 32

popd