Push-Location "libs\curl\include\curl\"
[Environment]::CurrentDirectory = $PWD
.\PREPARE.ps1
Pop-Location
[Environment]::CurrentDirectory = $PWD