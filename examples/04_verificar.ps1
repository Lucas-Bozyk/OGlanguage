# Execute da raiz: powershell -File examples/04_verificar.ps1
$ErrorActionPreference = 'Stop'
$raizProjeto = Split-Path -Parent $PSScriptRoot
Push-Location -LiteralPath $raizProjeto
try {
    & g++ -std=c++11 -Wall -Wextra -Werror -pedantic docs/04_afnog.cpp -o 04_afnog.exe
    if ($LASTEXITCODE -ne 0) { throw 'Falha na compilacao do modulo 4.' }
    & .\04_afnog.exe --testes
    if ($LASTEXITCODE -ne 0) { throw 'Falha nos testes do modulo 4.' }
} finally {
    Pop-Location
}
