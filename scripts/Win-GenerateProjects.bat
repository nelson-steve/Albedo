@echo off
pushd ..\
call dependencies\bin\premake\premake5.exe vs2022
popd\
PAUSE