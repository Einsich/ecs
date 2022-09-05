@echo off
set BUILD_TYPE=%1


cmake -G Ninja -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -B bin/%BUILD_TYPE%


cd bin/%BUILD_TYPE%

ninja


cd ../..