@echo off
set BUILD_TYPE=%1

cd sources/code_gen

cmake -G Ninja -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DBUILD_TYPE=%BUILD_TYPE% -B ../../code_gen/%BUILD_TYPE%


cd ../../code_gen/%BUILD_TYPE%

ninja


cd ../..