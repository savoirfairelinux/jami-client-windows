@echo off
setlocal EnableDelayedExpansion

set cloneSubmodules=N
if "%1" == "/c" (
    set cloneSubmodules=Y
)

set WGET_CMD=wget --no-check-certificate --retry-connrefused --waitretry=1 --read-timeout=20 --timeout=15 --tries=4
set APPLY_CMD=git apply --reject --ignore-whitespace --whitespace=fix

cd ..\
if exist qrencode-win32 (
    rmdir qrencode-win32 /s /q
)
git clone https://github.com/BlueDragon747/qrencode-win32.git
cd qrencode-win32
git checkout d6495a2aa74d058d54ae0f1b9e9e545698de66ce
%APPLY_CMD% ..\qrencode-win32.patch

:cleanup
endlocal
@endlocal
exit /B %ERRORLEVEL%

:getTarballDepToFolder
%WGET_CMD% %1/archive/%2.tar.gz
7z -y x %2.tar.gz && 7z -y x %2.tar
del %2.tar && del %2.tar.gz && del pax_global_header
if exist %3 (
    rmdir %3 /s /q
)
if exist %3-%2 (
    rename %3-%2 %3
) else (
    rename lib%3-%2 %3
)