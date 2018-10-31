@echo off
setlocal EnableDelayedExpansion

set cloneSubmodules=N
if "%1" == "/c" (
	set cloneSubmodules=Y
)

set SRC=%~dp0

set MSBUILD_ARGS=/nologo /p:useenv=true /p:Configuration=Release /p:Platform=x64 /verbosity:normal /maxcpucount:%NUMBER_OF_PROCESSORS%

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
call "%VSLATESTDIR%"\\VC\\Auxiliary\\Build\\vcvarsall.bat %Comp_x64%

if not exist "winsparkle" (
	git clone --depth=1 https://github.com/vslavik/winsparkle.git
)

if "%cloneSubmodules%" neq "N" (
    cd winsparkle
	git submodule init
	git submodule update
)

msbuild winsparkle/WinSparkle-2015.vcxproj %MSBUILD_ARGS%

:cleanup
endlocal
@endlocal
exit /B %ERRORLEVEL%