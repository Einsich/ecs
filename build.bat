@echo off
set BUILD_DIR=%1

if "%BUILD_DIR%" == "dbg" set BUILD_TYPE="Debug"
if "%BUILD_DIR%" == "dev" set BUILD_TYPE="RelWithDebInfo"
if "%BUILD_DIR%" == "rel" set BUILD_TYPE="Release"

cmake -G Ninja -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -B bin/%BUILD_DIR%

pushd bin\%BUILD_DIR%
ninja
popd