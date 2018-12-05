@echo off
setlocal EnableDelayedExpansion

set cloneSubmodules=N
if "%1" == "/c" (
    set cloneSubmodules=Y
)

set APPLY_CMD=git apply --reject --ignore-whitespace --whitespace=fix

if exist "winsparkle" (
    rmdir winsparkle /s /q
)
git clone --depth=1 https://github.com/vslavik/winsparkle.git
cd winsparkle
git checkout 6e7bb8554852d7f964fd176c440dd29cc772f2dc
%APPLY_CMD% ..\winsparkle.patch
if "%cloneSubmodules%" neq "N" (
    git submodule init
    git submodule update --depth=1
)
cd ..

if exist "qrencode-win32" (
    rmdir qrencode-win32 /s /q
)
git clone --depth=1 https://github.com/BlueDragon747/qrencode-win32.git
cd qrencode-win32
git checkout d6495a2aa74d058d54ae0f1b9e9e545698de66ce
%APPLY_CMD% ..\qrencode-win32.patch

:cleanup
endlocal
@endlocal
exit /B %ERRORLEVEL%