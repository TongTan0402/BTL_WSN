@echo off
setlocal enabledelayedexpansion
chcp 65001 >nul
cd /d "%~dp0"
echo Working directory: %CD%
echo.
call .\update_firmware\push_to_github.bat
if %ERRORLEVEL% NEQ 0 (
  echo Error: push_to_github.bat failed.
  pause
  endlocal
  exit /b %ERRORLEVEL%
)
endlocal
exit /b 0
