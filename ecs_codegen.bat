@echo off

set CWD=%cd%
cd code_gen/rel
ecs_code_gen.exe %CWD%/sources/tests
cd ../..