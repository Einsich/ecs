@echo off

set FORCE_REBUILD=%1

set CWD=%cd%
cd code_gen/rel
ecs_code_gen.exe %CWD%/sources/tests %FORCE_REBUILD%
cd ../..