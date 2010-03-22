@echo off
setlocal
IF "%1"=="clean" (
	rmdir /s "%~dp0build"
) ELSE (
	mkdir "%~dp0build" 2>nul
	cmake-gui -H"%CD%" -B"%~dp0build"
)
endlocal
