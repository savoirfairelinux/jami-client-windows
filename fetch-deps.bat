@echo off
setlocal EnableDelayedExpansion

set cloneSubmodules=N
if "%1" == "/c" (
	set cloneSubmodules=Y
)

if not exist "winsparkle" (
	git clone --depth=1 https://github.com/vslavik/winsparkle.git
)

if "%cloneSubmodules%" neq "N" (
    cd winsparkle
	git submodule init
	git submodule update
	cd ..
)

if not exist "qrencode-win32" (
	git clone --depth=1 https://github.com/BlueDragon747/qrencode-win32.git
)

:cleanup
endlocal
@endlocal
exit /B %ERRORLEVEL%