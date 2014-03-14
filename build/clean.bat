@echo off

cd ..

for /r %%a in (.) do (
set list=%%a
setlocal enabledelayedexpansion
if "!list:~-8,6!"=="-Debug" rd /q /s "!list:~0,-2!"
if "!list:~-10,8!"=="-Release" rd /q /s "!list:~0,-2!"
endlocal
)

rd /q /s "build/!bin"
rd /q /s "build/!tmp"
rd /q /s "build/debug"
rd /q /s "build/release"

del /f /q /s /a *.user *.ncb *.suo *.sdf *.i