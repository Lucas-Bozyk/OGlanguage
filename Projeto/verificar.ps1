# Localiza CMake inclusive quando ele veio com o Visual Studio e nao esta no PATH.
param([ValidateSet('Debug', 'Release')][string]$Configuracao = 'Release')
$ErrorActionPreference = 'Stop'
$comandoCmake = Get-Command cmake -ErrorAction SilentlyContinue
if ($comandoCmake) {
    $cmakeExecutavel = $comandoCmake.Source
} else {
    $cmakeExecutavel = Join-Path $env:ProgramFiles 'CMake/bin/cmake.exe'
    if (-not (Test-Path -LiteralPath $cmakeExecutavel)) {
        $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
        if (Test-Path -LiteralPath $vswhere) {
            $encontrados = @(& $vswhere -latest -prerelease -products '*' -find 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe')
            if ($encontrados.Count -gt 0) { $cmakeExecutavel = $encontrados[0] }
        }
    }
}
if (-not (Test-Path -LiteralPath $cmakeExecutavel)) {
    throw 'CMake nao encontrado. Instale CMake ou as ferramentas CMake do Visual Studio.'
}

$compilador = Get-Command g++ -ErrorAction SilentlyContinue
if (-not $compilador) { throw 'g++ nao encontrado no PATH.' }
$comandoNinja = Get-Command ninja -ErrorAction SilentlyContinue
if ($comandoNinja) {
    $ninjaExecutavel = $comandoNinja.Source
} else {
    $pastaCmake = Split-Path -Parent $cmakeExecutavel
    $ninjaExecutavel = Join-Path $pastaCmake '../../Ninja/ninja.exe'
}
$gerador = @()
if (Test-Path -LiteralPath $ninjaExecutavel) {
    $gerador = @('-G', 'Ninja', "-DCMAKE_MAKE_PROGRAM=$ninjaExecutavel")
} else {
    $makeExecutavel = Join-Path (Split-Path -Parent $compilador.Source) 'mingw32-make.exe'
    if (-not (Test-Path -LiteralPath $makeExecutavel)) {
        throw 'Ninja ou mingw32-make nao encontrado para compilar com g++.'
    }
    $gerador = @('-G', 'MinGW Makefiles', "-DCMAKE_MAKE_PROGRAM=$makeExecutavel")
}

$pastaBuild = Join-Path $PSScriptRoot 'build'
& $cmakeExecutavel -S $PSScriptRoot -B $pastaBuild @gerador `
    "-DCMAKE_CXX_COMPILER=$($compilador.Source)" "-DCMAKE_BUILD_TYPE=$Configuracao" -DBUILD_TESTING=ON
if ($LASTEXITCODE -ne 0) { throw 'Falha na configuracao CMake.' }
& $cmakeExecutavel --build $pastaBuild --config $Configuracao --parallel
if ($LASTEXITCODE -ne 0) { throw 'Falha na compilacao.' }
$ctestExecutavel = Join-Path (Split-Path -Parent $cmakeExecutavel) 'ctest.exe'
& $ctestExecutavel --test-dir $pastaBuild --build-config $Configuracao --output-on-failure
if ($LASTEXITCODE -ne 0) { throw 'Falha nos testes.' }
