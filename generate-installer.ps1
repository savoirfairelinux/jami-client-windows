[cmdletbinding()]
param (
    [string]$QtIFWDir = "C:\Qt\Tools\QtInstallerFramework\3.0\bin"
);

Write-Host "Generating Qt Installer..."

$ClientDir = Split-Path -parent $MyInvocation.MyCommand.Definition
$FilesLocation = $ClientDir + "\x64\Release"
$DataDir = $ClientDir + "\packages\net.jami\data"

Write-Host "Copying required files to Qt Installer directory"

$FilesToCopy = @(
    "$FilesLocation\Jami.exe"
    "$FilesLocation\jami.ico"
    "$FilesLocation\qtwebengine_resources.pak"
    "$FilesLocation\QtWebEngineProcess.exe"
    "$FilesLocation\icudtl.dat"
    "$FilesLocation\*.dll"
)
$CopyDir = $DataDir
New-Item -ItemType directory -Path $CopyDir -Force
ForEach ($i in $FilesToCopy) {
    Write-Host "Copying: " $i " => " $CopyDir
    Copy-Item -Path $i -Destination $CopyDir -Force
}

$FilesToCopy = @(
    "$FilesLocation\platforms\*"
)
$CopyDir = $DataDir + "\platforms"
New-Item -ItemType directory -Path $CopyDir -Force
ForEach ($i in $FilesToCopy) {
    Write-Host "Copying: " $i " => " $CopyDir
    Copy-Item -Path $i -Destination $CopyDir -Force
}

$FilesToCopy = @(
    "$FilesLocation\imageformats\*"
)
$CopyDir = $DataDir + "\imageformats"
New-Item -ItemType directory -Path $CopyDir -Force
ForEach ($i in $FilesToCopy) {
    Write-Host "Copying: " $i " => " $CopyDir
    Copy-Item -Path $i -Destination $CopyDir -Force
}

$FilesToCopy = @(
    "$FilesLocation\sqldrivers\*"
)
$CopyDir = $DataDir + "\sqldrivers"
New-Item -ItemType directory -Path $CopyDir -Force
ForEach ($i in $FilesToCopy) {
    Write-Host "Copying: " $i " => " $CopyDir
    Copy-Item -Path $i -Destination $CopyDir -Force
}

$FilesToCopy = @(
    "$FilesLocation\ringtones\*"
)
$CopyDir = $DataDir + "\ringtones"
New-Item -ItemType directory -Path $CopyDir -Force
ForEach ($i in $FilesToCopy) {
    Write-Host "Copying: " $i " => " $CopyDir
    Copy-Item -Path $i -Destination $CopyDir -Force
}

$FilesToCopy = @(
    "$FilesLocation\share\ring\translations\*"
)
$CopyDir = $DataDir + "\share\ring\translations"
New-Item -ItemType directory -Path $CopyDir -Force
ForEach ($i in $FilesToCopy) {
    Write-Host "Copying: " $i " => " $CopyDir
    Copy-Item -Path $i -Destination $CopyDir -Force
}

$FilesToCopy = @(
    "$FilesLocation\share\libringclient\translations\*"
)
$CopyDir = $DataDir + "\share\libringclient\translations"
New-Item -ItemType directory -Path $CopyDir -Force
ForEach ($i in $FilesToCopy) {
    Write-Host "Copying: " $i " => " $CopyDir
    Copy-Item -Path $i -Destination $CopyDir -Force
}

Write-Host "Generating installer file"
Set-Location -Path $ClientDir
$Command = $QtIFWDir + "\binarycreator.exe"
$Parameters = '-c', 'config\config.xml', '-p', 'packages', 'jami-installer'
& $Command $Parameters