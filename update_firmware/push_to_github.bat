@echo off
setlocal enabledelayedexpansion
chcp 65001 >nul

echo ======================================================
echo        ESP32 OTA Firmware Update Script
echo ======================================================
echo. 

:: Lấy thư mục hiện tại của script
set "SCRIPT_DIR=%~dp0"

:: Đi tới thư mục update_firmware
cd /d "%SCRIPT_DIR%"
echo Working directory: %CD%
echo.

:: Tạo thư mục firmware nếu chưa có
if not exist "firmware" (
    echo Creating firmware directory...
    mkdir firmware
)

:: Xóa file firmware.bin cũ trong thư mục firmware (nếu có)
if exist "firmware\firmware.bin" (
    echo Deleting old firmware.bin...
    del /f /q "firmware\firmware.bin" 2>nul
    if %ERRORLEVEL% EQU 0 (
        echo Old firmware.bin deleted successfully
    ) else (
        echo Warning: Could not delete old firmware.bin
    )
)

timeout /t 1 >nul

echo Copying firmware.bin from build directory...
copy "..\.pio\build\esp32dev\firmware.bin" "firmware\" 
if %ERRORLEVEL% NEQ 0 (
    echo Error: Failed to copy firmware.bin
    echo Please make sure the project is built first: pio run
    pause
    exit /b 1
)
echo Copy successful!

:: Chuyển vào thư mục firmware (nơi có .git)
cd firmware
echo Changed to firmware directory: %CD%
echo.

:: Kiểm tra xem có git repository không
if not exist ".git" (
    echo Error: firmware directory is not a git repository!
    echo Please run: git clone https://github.com/TongTan0402/firmware_esp32_ota.git firmware
    pause
    cd ..
    exit /b 1
)

:: Lấy version hiện tại từ file ver.txt
set "CURRENT_VERSION=0"
if exist "ver.txt" (
    set /p CURRENT_VERSION=<ver.txt
    echo Current version: !CURRENT_VERSION!
) else (
    echo No version file found, starting from version 0
)

:: Tính version tiếp theo
set /a NEXT_VERSION=!CURRENT_VERSION!+1
echo Next version: !NEXT_VERSION!
echo.

:: Cập nhật file version
echo !NEXT_VERSION! > ver.txt

:: Add files tới git
git add firmware.bin ver.txt
if %ERRORLEVEL% NEQ 0 (
    echo Error: git add failed
    cd ..
    pause
    exit /b 1
)

:: Commit changes
git commit -m "ver !NEXT_VERSION!"
if %ERRORLEVEL% NEQ 0 (
    echo Error: git commit failed
    cd ..
    pause
    exit /b 1
)

echo Pushing to GitHub...
:: Push tới GitHub
git push origin main
if %ERRORLEVEL% NEQ 0 (
    echo Error: git push failed
    echo Please check your GitHub credentials and internet connection.
    cd ..
    pause
    exit /b 1
)

:: Quay lại thư mục update_firmware
cd ..
if %ERRORLEVEL% NEQ 0 (
    echo Error: git push failed
    echo Please check your GitHub credentials and internet connection.
    pause
    exit /b 1
)

echo.
echo ======================================================
echo   Version update completed successfully!
echo   Previous version: !CURRENT_VERSION!
echo   Current version: !NEXT_VERSION!
echo ======================================================
echo.

:: Chạy file mqtt_start.py
echo Starting mqtt_start.py...
if exist "mqtt_start.py" (
    python mqtt_start.py
    if %ERRORLEVEL% NEQ 0 (
        echo Warning: mqtt_start.py exited with error code %ERRORLEVEL%
    ) else (
        echo mqtt_start.py completed successfully
    )
) else (
    echo Warning: mqtt_start.py not found in current directory
    echo Looking for mqtt_start.py in parent directory...
    if exist "..\mqtt_start.py" (
        python ..\mqtt_start.py
        if %ERRORLEVEL% NEQ 0 (
            echo Warning: mqtt_start.py exited with error code %ERRORLEVEL%
        ) else (
            echo mqtt_start.py completed successfully
        )
    ) else (
        echo Error: mqtt_start.py not found
    )
)

echo.
echo ======================================================
echo   All tasks completed!
echo ======================================================
echo.

endlocal
exit /b 0