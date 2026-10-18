@echo off
REM ===========================================================================
REM  snake\build.cmd  --  one-click build + run
REM
REM  This file does exactly what these three commands do by hand:
REM     cmake -S . -B build -G "Visual Studio 17 2022" -A x64
REM     cmake --build build --config Debug
REM     build\Debug\snake.exe
REM
REM  It only exists because your CMake lives inside Visual Studio and is not
REM  on PATH yet, so the full path is long to type. Nothing here is magic.
REM ===========================================================================

set "CMAKE=D:\vs2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"

echo [1/3] configure ...
"%CMAKE%" -S "%~dp0." -B "%~dp0build" -G "Visual Studio 17 2022" -A x64
if errorlevel 1 goto :failed

echo.
echo [2/3] build ...
"%CMAKE%" --build "%~dp0build" --config Debug
if errorlevel 1 goto :failed

echo.
echo [3/3] run ...
echo ---------------------------------------------------------------
"%~dp0build\Debug\snake.exe"
echo ---------------------------------------------------------------
pause
exit /b 0

:failed
echo.
echo ============ BUILD FAILED - read the red text above ============
pause
exit /b 1
