#IMPORTANT!!!
# WE GENERATED BOTH THE generator.cpp and this File Using claude code.
# These both files help generate the datapoints and store in the postgresql database.
# The prompt we use was "Help use generate 100000 data points and store them in the PostgreSQL database" using claude code


# generate.ps1
# Builds the workload generator (only if needed) and fills the database.
#
# Usage:
#   .\generate.ps1                 -> 1,000,000 requests per workload
#   .\generate.ps1 -Count 100000   -> smaller run
#IMPORTANT: Make sure PostgreSQL is running and accessible before running this script.
param([int]$Count = 1000000)

$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot   # always run from the repo folder

$source = "generation/generator.cpp"
$exe    = "generate_workloads.exe"

# 1. Make sure the compiler exists
if (-not (Get-Command g++ -ErrorAction SilentlyContinue)) {
    Write-Error "g++ not found. Install MSYS2 and add C:\msys64\ucrt64\bin to your Path."
}

# 2. Rebuild only when the .cpp is newer than the .exe
if (-not (Test-Path $exe) -or (Get-Item $source).LastWriteTime -gt (Get-Item $exe).LastWriteTime) {
    Write-Host "Building $exe..."
    g++ -O2 -std=c++17 $source -o $exe -lpq
    if ($LASTEXITCODE -ne 0) { Write-Error "Build failed." }
}

# 3. Ask for the password without showing it
$setPassword = $false
if (-not $env:PGPASSWORD) {
    $secure = Read-Host "PostgreSQL password" -AsSecureString
    $env:PGPASSWORD = [System.Net.NetworkCredential]::new("", $secure).Password
    $setPassword = $true
}

# 4. Generate the data
try {
    & ".\$exe" --count $Count
}
finally {
    if ($setPassword) { Remove-Item Env:PGPASSWORD }   # don't leave the password lying around
}