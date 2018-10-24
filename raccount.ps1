[cmdletbinding()] 

param (
    [switch]$Backup,
    [switch]$Purge,
    [switch]$Restore
);

$ParametersPassed = $PSBoundParameters.Count

if ($ParametersPassed -eq 1) {
    if ($Backup) {
        Write-Host "Backing up..."
        Copy-Item -Path $env:USERPROFILE"\.config\ring" -Destination $env:USERPROFILE"\.bak\.config\ring" -Force -Recurse
        Copy-Item -Path $env:USERPROFILE"\.cache\ring" -Destination $env:USERPROFILE"\.bak\.cache\ring" -Force -Recurse
        Copy-Item -Path $env:USERPROFILE"\.local\share\ring" -Destination $env:USERPROFILE"\.bak\.local\share\ring" -Force -Recurse
        Copy-Item -Path $env:USERPROFILE"\AppData\Local\Savoir-faire Linux\Ring" -Destination $env:USERPROFILE"\.bak\AppData\Local\Savoir-faire Linux\Ring" -Force -Recurse
        Copy-Item -Path $env:USERPROFILE"\AppData\Local\Ring" -Destination $env:USERPROFILE"\.bak\AppData\Local\Ring" -Force -Recurse
    }
    elseif ($Purge) {
        Write-Host "Purging..."
        # purges Ring data
        Remove-Item $env:USERPROFILE"\.config\ring" -Force -Recurse
        Remove-Item $env:USERPROFILE"\.cache\ring" -Force -Recurse
        Remove-Item $env:USERPROFILE"\.local\share\ring" -Force -Recurse
        Remove-Item $env:USERPROFILE"\AppData\Local\Savoir-faire Linux\Ring" -Force -Recurse
        Remove-Item $env:USERPROFILE"\AppData\Local\Ring" -Force -Recurse
    }
    elseif ($Restore) {
        Write-Host "Restoring..."
        Copy-Item -Path $env:USERPROFILE"\.bak\.config\ring" -Destination $env:USERPROFILE"\.config" -Force -Recurse
        Copy-Item -Path $env:USERPROFILE"\.bak\.cache\ring" -Destination $env:USERPROFILE"\.cache" -Force -Recurse
        Copy-Item -Path $env:USERPROFILE"\.bak\.local\share\ring" -Destination $env:USERPROFILE"\.local\share" -Force -Recurse
        Copy-Item -Path $env:USERPROFILE"\.bak\AppData\Local\Savoir-faire Linux\Ring" -Destination $env:USERPROFILE"\AppData\Local\Savoir-faire Linux" -Force -Recurse
        Copy-Item -Path $env:USERPROFILE"\.bak\AppData\Local\Ring" -Destination $env:USERPROFILE"\AppData\Local" -Force -Recurse
    }
}
else {
    Write-Host "Invalid options"
}