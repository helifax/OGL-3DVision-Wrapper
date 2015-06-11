@echo off
echo =============================================================
echo === Star Wars: Knights Of The Old Republic II - 3D Vision Patch
echo =============================================================
echo.
echo.
echo This will uninstall the Wrapper! Are you sure you want to proccede ?
SET /P M=Type Yes, No then press ENTER:
IF %M%==Yes GOTO DELETE
IF %M%==No GOTO NOPE
IF %M%==yes GOTO DELETE
IF %M%==no GOTO NOPE

:DELETE
echo.
echo.
echo.
echo Deleting files....
del "3DVisionWrapper.log"
del "3DVisionWrapper.ini"
del "Logo.bmp"
del "opengl32.dll"
del "Profile.nip"
echo.
echo.
echo Operation Complete!
PAUSE
( del /q /f "%~f0" >nul 2>&1 & exit /b 0  )

:NOPE
echo.
echo.
echo Wrapper Not Deleted ! Please Enjoy it:)
echo.
echo.
PAUSE

