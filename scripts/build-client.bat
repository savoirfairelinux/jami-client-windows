:: Jami - native Windows client project build script

@echo off
setlocal

if "%1" == "/?" goto Usage
if "%~1" == "" goto Usage

set doDeps=N
set doCompile=N
set doBuild=N
set doBuildBeta=N

set SCRIPTNAME=%~nx0

if "%1"=="compile" (
    set doCompile=Y
) else if "%1"=="build" (
    set doBuild=Y
) else if "%1"=="deps" (
    set doDeps=Y
) else if "%1"=="beta" (
    set doBuildBeta=Y
) else (
    goto Usage
)

set arch=N

shift
:ParseArgs
if "%1" == "" goto FinishedArgs
if /I "%1"=="x86" (
    set arch=x86
) else if /I "%1"=="x64" (
    set arch=x64
) else (
    goto Usage
)
shift
goto ParseArgs

:FinishedArgs
if "%arch%"=="x86" (
    set MSBUILD_ARGS=/nologo /p:useenv=true /p:Platform=Win32 /maxcpucount:%NUMBER_OF_PROCESSORS%
) else if "%arch%"=="x64" (
    set MSBUILD_ARGS=/nologo /p:useenv=true /p:Platform=x64 /maxcpucount:%NUMBER_OF_PROCESSORS%
)

@setlocal

set VSInstallerFolder="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer"
if %PROCESSOR_ARCHITECTURE%==x86 set VSInstallerFolder="%ProgramFiles%\Microsoft Visual Studio\Installer"

pushd %VSInstallerFolder%
for /f "usebackq tokens=*" %%i in (`vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
  set VSLATESTDIR=%%i
)
popd

echo VS Installation folder: %VSLATESTDIR%

if not exist "%VSLATESTDIR%\VC\Auxiliary\Build\vcvarsall.bat" (
    echo:
    echo VSInstallDir not found or not installed correctly.
    goto cleanup
)

if %PROCESSOR_ARCHITECTURE%==x86 (
    set Comp_x86=x86 10.0.15063.0
    set Comp_x64=x86_amd64 10.0.15063.0
) else (
    set Comp_x86=amd64_x86 10.0.15063.0
    set Comp_x64=amd64 10.0.15063.0
)

set path=%path:"=%
if "%arch%"=="x86" (
    call "%VSLATESTDIR%"\\VC\\Auxiliary\\Build\\vcvarsall.bat %Comp_x86%
) else if "%arch%"=="x64" (
    call "%VSLATESTDIR%"\\VC\\Auxiliary\\Build\\vcvarsall.bat %Comp_x64%
)

if "%arch%" neq "N" (
    if "%doCompile%" neq "N" (
        goto compileClient
    ) else if "%doBuild%" neq "N" (
        goto buildClient
    ) else if "%doDeps%" neq "N" (
        goto buildDeps
    ) else if "%doBuildBeta%" neq "N" (
        goto buildBetaClient
    )
    goto :eof
)
goto Usage

:buildDeps
cd ..\
set TOBUILD=..\qrencode-win32\qrencode-win32\vc8\qrcodelib\qrcodelib.vcxproj
msbuild %TOBUILD% /verbosity:normal /p:Configuration=Release-Lib %MSBUILD_ARGS%
set WGET_CMD=wget --no-check-certificate --retry-connrefused --waitretry=1 --read-timeout=20 --timeout=15 --tries=4
goto cleanup

:compileClient
cd ..\
msbuild ring-client-windows.vcxproj /verbosity:normal /p:Configuration=ReleaseCompile %MSBUILD_ARGS%
goto cleanup

:buildClient
cd ..\
msbuild ring-client-windows.vcxproj /verbosity:normal /p:Configuration=Release %MSBUILD_ARGS%
goto cleanup

:buildBetaClient
cd ..\
msbuild ring-client-windows.vcxproj /verbosity:normal /p:Configuration=Beta %MSBUILD_ARGS%
goto cleanup

@endlocal

:Usage
echo:
echo The correct usage is:
echo:
echo     %0 [action] [architecture]
echo:
echo where
echo:
echo [action]           is: compile  ^| build
echo [architecture]     is: x86      ^| x64
echo:
echo For example:
echo     %0 compile x86    - compile only x86 (for CI)
echo     %0 build x64      - build x64 client
echo:
goto :eof

:cleanup
endlocal
exit /B %ERRORLEVEL%