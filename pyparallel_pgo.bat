@echo off
set PATH=%~dp0\PCBuild\x64-pgo:%PATH%
set PYTHONPATH=%~dp0\Lib
call "%VS100COMNTOOLS%\..\..\VC\vcvarsall.bat" x86_amd64
PCbuild\x64-pgo\python.exe %*
