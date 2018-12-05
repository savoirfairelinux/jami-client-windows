@echo off
setlocal EnableDelayedExpansion

set cloneSubmodules=N
if "%1" == "/c" (
    set cloneSubmodules=Y
)

set WGET_CMD=wget --no-check-certificate --retry-connrefused --waitretry=1 --read-timeout=20 --timeout=15 --tries=4
set APPLY_CMD=git apply --reject --ignore-whitespace --whitespace=fix

if exist "winsparkle" (
    rmdir winsparkle /s /q
)
call :getTarballDepToFolder https://github.com/vslavik/winsparkle 6e7bb8554852d7f964fd176c440dd29cc772f2dc "winsparkle"
cd winsparkle
%APPLY_CMD% ..\winsparkle.patch
if "%cloneSubmodules%" neq "N" (
    cd 3rdparty

    call :getTarballDepToFolder https://github.com/libexpat/libexpat 97c6bd01990090d4015364ae37dd141f3c39a30f expat
    call :getTarballDepToFolder https://github.com/openssl/openssl e5bba24cd8bb3e5127a578b85c6edf013a38ea6d openssl
    call :getTarballDepToFolder https://github.com/wxWidgets/wxWidgets 08ea09c7fee69984c75e51ea8ee90ef075f06e90 wxWidgets

    cd wxWidgets\3rdparty
    call :getTarballDepToFolder https://github.com/wxWidgets/Catch 011f6e6458d888246f94643e293f002073cff489 catch
    cd ..\..

    cd ..
)
cd ..

if exist "qrencode-win32" (
    rmdir qrencode-win32 /s /q
)
call :getTarballDepToFolder https://github.com/BlueDragon747/qrencode-win32 d6495a2aa74d058d54ae0f1b9e9e545698de66ce qrencode-win32
cd qrencode-win32
%APPLY_CMD% ..\qrencode-win32.patch

:cleanup
endlocal
@endlocal
exit /B %ERRORLEVEL%

:getTarballDepToFolder
%WGET_CMD% %1/archive/%2.tar.gz
7z -y x %2.tar.gz && 7z -y x %2.tar
del %2.tar && del %2.tar.gz && del pax_global_header
rmdir %3 /s /q
if exist %3-%2 (
    rename %3-%2 %3
) else (
    rename lib%3-%2 %3
)