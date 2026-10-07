param([ValidateSet('Release', 'Debug')][string]$Configuration = 'Release')
$ErrorActionPreference = 'Stop'
$portableCMake = Join-Path $PSScriptRoot '.tools\cmake-3.31.6-windows-x86_64\bin\cmake.exe'
$portableCompilerDirectory = Join-Path $PSScriptRoot '.tools\w64devkit\bin'
$previousPath = $env:PATH
try {
    if ((Test-Path -LiteralPath $portableCMake) -and
        (Test-Path -LiteralPath (Join-Path $portableCompilerDirectory 'g++.exe'))) {
        $env:PATH = $portableCompilerDirectory + ';' + $env:PATH
        $buildDirectory = Join-Path $PSScriptRoot 'build-mingw'
        & $portableCMake -S $PSScriptRoot -B $buildDirectory -G 'MinGW Makefiles' "-DCMAKE_BUILD_TYPE=$Configuration" '-DCMAKE_MAKE_PROGRAM=make.exe'
        if ($LASTEXITCODE -ne 0) { throw 'CMake configuration failed.' }
        & $portableCMake --build $buildDirectory --parallel 2
        if ($LASTEXITCODE -ne 0) { throw 'Compilation failed.' }
        Write-Host "Executable: $buildDirectory\Lab1.exe"
    } else {
        $cmakeCommand = Get-Command cmake -ErrorAction Stop
        $buildDirectory = Join-Path $PSScriptRoot 'build'
        & $cmakeCommand.Source -S $PSScriptRoot -B $buildDirectory -G 'Visual Studio 17 2022' -A x64
        if ($LASTEXITCODE -ne 0) { throw 'CMake configuration failed.' }
        & $cmakeCommand.Source --build $buildDirectory --config $Configuration
        if ($LASTEXITCODE -ne 0) { throw 'Compilation failed.' }
        Write-Host "Executable: $buildDirectory\$Configuration\Lab1.exe"
    }
} finally {
    $env:PATH = $previousPath
}
