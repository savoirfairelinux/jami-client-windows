$CurrentDir = Convert-Path .
#$QtDir = "C:\Qt\5.9.4\msvc2017_64\bin"


$FilesToMove = @(
    "$CurrentDir\winsparkle\x64\Release\WinSparkle.dll",
    "$CurrentDir\..\daemon\contrib\build\ffmpeg\Build\win32\x64\bin\avcodec-58.dll",
    "$CurrentDir\..\daemon\contrib\build\ffmpeg\Build\win32\x64\bin\avutil-56.dll",
    "$CurrentDir\..\daemon\contrib\build\ffmpeg\Build\win32\x64\bin\avformat-58.dll",
    "$CurrentDir\..\daemon\contrib\build\ffmpeg\Build\win32\x64\bin\avdevice-58.dll",
    "$CurrentDir\..\daemon\contrib\build\restbed\dependency\openssl\out32dll\libeay32.dll",
    "$CurrentDir\..\daemon\contrib\build\restbed\dependency\openssl\out32dll\ssleay32.dll",
    "$CurrentDir\..\daemon\contrib\build\ffmpeg\Output\win32\x64\libswresample\swresample-3.dll",
    "$CurrentDir\..\daemon\contrib\build\ffmpeg\Build\win32\x64\bin\swscale-5.dll",
    "$CurrentDir\..\daemon\contrib\build\ffmpeg\Build\win32\x64\bin\avfilter-7.dll"
#    "$QtDir\Qt5Widgets.dll",
#    "$QtDir\Qt5WinExtras.dll",
#    "$QtDir\Qt5Gui.dll"
    )



foreach ($i in $FilesToMove){
    Copy-Item $i -Destination $CurrentDir
}

write-host "move completed" -NoNewline