[cmdletbinding()]
param (
    [string]$daemonDir,
    [string]$lrcDir
);

write-host "copying runtime files..."

$QtDir = "C:\Qt\5.9.4\msvc2017_64"
$ClientDir = split-path -parent $MyInvocation.MyCommand.Definition

$OutDir = $ClientDir + "\x64\Release"
New-Item -ItemType directory -Path $OutDir -Force

if (-not ($daemonDir)) {
    $daemonDir = '$ClientDir\..\daemon'
}

if (-not ($lrcDir)) {
    $lrcDir = '$ClientDir\..\lrc'
}

# dependency bin files
$FilesToCopy = @(
    "$daemonDir\contrib\build\ffmpeg\Build\win32\x64\bin\avcodec-58.dll",
    "$daemonDir\contrib\build\ffmpeg\Build\win32\x64\bin\avutil-56.dll",
    "$daemonDir\contrib\build\ffmpeg\Build\win32\x64\bin\avformat-58.dll",
    "$daemonDir\contrib\build\ffmpeg\Build\win32\x64\bin\avdevice-58.dll",
    "$daemonDir\contrib\build\ffmpeg\Build\win32\x64\bin\swresample-3.dll",
    "$daemonDir\contrib\build\ffmpeg\Build\win32\x64\bin\swscale-5.dll",
    "$daemonDir\contrib\build\ffmpeg\Build\win32\x64\bin\avfilter-7.dll",
    "$daemonDir\contrib\build\restbed\dependency\openssl\out32dll\libeay32.dll",
    "$daemonDir\contrib\build\restbed\dependency\openssl\out32dll\ssleay32.dll",
    "$ClientDir\winsparkle\x64\Release\WinSparkle.dll",
    "$ClientDir\ring.nsi",
    "$ClientDir\images\jami.ico"
    "$ClientDir\License.rtf"
    )
foreach ($i in $FilesToCopy) {
    write-host "copying: " $i " => " $OutDir
    Copy-Item -Path $i -Destination $OutDir -Force
}

############
# qt
############

# qt bin files
$FilesToCopy = @(
    "$QtDir\bin\Qt5Core.dll",
    "$QtDir\bin\Qt5Gui.dll",
    "$QtDir\bin\Qt5Network.dll",
    "$QtDir\bin\Qt5Positioning.dll",
    "$QtDir\bin\Qt5PrintSupport.dll",
    "$QtDir\bin\Qt5Qml.dll",
    "$QtDir\bin\Qt5Quick.dll",
    "$QtDir\bin\Qt5QuickWidgets.dll",
    "$QtDir\bin\Qt5Sql.dll",
    "$QtDir\bin\Qt5Svg.dll",
    "$QtDir\bin\Qt5WebChannel.dll",
    "$QtDir\bin\Qt5WebEngine.dll",
    "$QtDir\bin\Qt5WebEngineCore.dll",
    "$QtDir\bin\Qt5WebEngineWidgets.dll",
    "$QtDir\bin\Qt5Widgets.dll",
    "$QtDir\bin\Qt5WinExtras.dll",
    "$QtDir\bin\Qt5Xml.dll",
    "$QtDir\bin\Qt5Multimedia.dll",
    "$QtDir\bin\Qt5MultimediaWidgets.dll",
    "$QtDir\bin\Qt5OpenGL.dll",
    "$QtDir\bin\libEGL.dll",
    "$QtDir\bin\libGLESv2.dll",
    "$QtDir\bin\QtWebEngineProcess.exe",
    "$QtDir\resources\qtwebengine_resources.pak",
    "$QtDir\resources\icudtl.dat"
    )
foreach ($i in $FilesToCopy) {
    write-host "copying: " $i " => " $OutDir
    Copy-Item -Path $i -Destination $OutDir -Force
}

# qt imageformats
$FilesToCopy = @(
    "$QtDir\plugins\imageformats\qgif.dll",
    "$QtDir\plugins\imageformats\qico.dll",
    "$QtDir\plugins\imageformats\qjpeg.dll",
    "$QtDir\plugins\imageformats\qsvg.dll"
    )
$CopyDir = $OutDir + "\imageformats"
New-Item -ItemType directory -Path $CopyDir -Force
foreach ($i in $FilesToCopy) {
    write-host "copying: " $i " => " $CopyDir
    Copy-Item -Path $i -Destination $CopyDir -Force
}

# qt platform dll for windows
$CopyDir = $OutDir + "\platforms"
New-Item -ItemType directory -Path $CopyDir -Force
$file = "$QtDir\plugins\platforms\qwindows.dll"
write-host "copying: " $file " => " $CopyDir
Copy-Item -Path $file -Destination $CopyDir -Force

# qt sql driver
$CopyDir = $OutDir + "\sqldrivers"
New-Item -ItemType directory -Path $CopyDir -Force
$file = "$QtDir\plugins\sqldrivers\qsqlite.dll"
write-host "copying: " $file " => " $CopyDir
Copy-Item -Path $file -Destination $CopyDir -Force

# ringtones
$CopyDir = $OutDir + "\ringtones"
New-Item -ItemType directory -Path $CopyDir -Force
$RingtonePath = "$ClientDir\..\daemon\ringtones"
write-host "copying ringtones..."
Get-ChildItem -Path $RingtonePath -Include *.ul, *.wav, *.ogg -Recurse | ForEach-Object {
    write-host "copying ringtone: " $_.FullName " => " $CopyDir
    Copy-Item -Path $_.FullName -Destination $CopyDir -Force –Recurse
}

# qt translations
$lrelease = "$QtDir\bin\lrelease.exe"

# lrc translations
$lrcTSPath = "$lrcDir\translations"
Get-ChildItem -Path $lrcTSPath -Include *.ts -Recurse | ForEach-Object {
    & $lrelease $_.FullName
}
$CopyDir = $OutDir + "\share\libringclient\translations"
New-Item -ItemType directory -Path $CopyDir -Force
write-host "copying ringtones..."
Get-ChildItem -Path $lrcTSPath -Include *.qm -Recurse | ForEach-Object {
    write-host "copying translation file: " $_.FullName " => " $CopyDir
    Copy-Item -Path $_.FullName -Destination $CopyDir -Force –Recurse
}

# client translations
$clientTSPath = "$ClientDir\translations"
Get-ChildItem -Path $clientTSPath -Include *.ts -Recurse | ForEach-Object {
    & $lrelease $_.FullName
}
$CopyDir = $OutDir + "\share\ring\translations"
New-Item -ItemType directory -Path $CopyDir -Force
write-host "copying ringtones..."
Get-ChildItem -Path $lrcTSPath -Include *.qm -Recurse | ForEach-Object {
    write-host "copying translation file: " $_.FullName " => " $CopyDir
    Copy-Item -Path $_.FullName -Destination $CopyDir -Force –Recurse
}

write-host "copy completed" -NoNewline