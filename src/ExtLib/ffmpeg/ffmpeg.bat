@ECHO OFF
REM $Id: ffmpeg.bat 3611 2013-10-23 07:14:42Z aleksoid $
REM
REM (C) 2009-2013 see Authors.txt
REM
REM This file is part of MPC-BE.
REM
REM MPC-BE is free software; you can redistribute it and/or modify
REM it under the terms of the GNU General Public License as published by
REM the Free Software Foundation; either version 3 of the License, or
REM (at your option) any later version.
REM
REM MPC-BE is distributed in the hope that it will be useful,
REM but WITHOUT ANY WARRANTY; without even the implied warranty of
REM MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
REM GNU General Public License for more details.
REM
REM You should have received a copy of the GNU General Public License
REM along with this program.  If not, see <http://www.gnu.org/licenses/>.

SETLOCAL
PUSHD %~dp0

IF /I "%~1"=="help"   GOTO SHOWHELP
IF /I "%~1"=="/help"  GOTO SHOWHELP
IF /I "%~1"=="-help"  GOTO SHOWHELP
IF /I "%~1"=="--help" GOTO SHOWHELP
IF /I "%~1"=="/?"     GOTO SHOWHELP

IF DEFINED MINGW32 GOTO VarOk
ECHO ERROR: Please define MINGW32 (and/or MSYS) environment variable(s)
ENDLOCAL
EXIT /B

:VarOk
SET PATH=%MSYS%\bin;%MINGW32%\bin;%PATH%

SET "BUILDTYPE=build"
SET "VS=VS2013=yes"

SET ARG=%*
SET ARG=%ARG:/=%
SET ARG=%ARG:-=%

FOR %%A IN (%ARG%) DO (
	IF /I "%%A" == "clean" SET "BUILDTYPE=clean"
	IF /I "%%A" == "rebuild" SET "BUILDTYPE=rebuild"
	IF /I "%%A" == "64" SET "BIT=64BIT=yes"
	IF /I "%%A" == "Debug" SET "DEBUG=DEBUG=yes"
	IF /I "%%A" == "VS2010" SET "VS=VS2010=yes"
	IF /I "%%A" == "VS2012" SET "VS=VS2012=yes"
)

IF /I "%BUILDTYPE%" == "rebuild" (
  SET "BUILDTYPE=clean"
  CALL :SubMake clean
  SET "BUILDTYPE=build"
  CALL :SubMake
  EXIT /B
) ELSE (
  CALL :SubMake
  EXIT /B
)

:SubMake
IF "%BUILDTYPE%" == "clean" (
  SET JOBS=1
) ELSE (
  SET "BUILDTYPE="
  IF DEFINED NUMBER_OF_PROCESSORS (
    SET JOBS=%NUMBER_OF_PROCESSORS%
  ) ELSE (
    SET JOBS=4
  )
)

make.exe -f ffmpeg.mak %BUILDTYPE% -j%JOBS% %BIT% %DEBUG% %VS%

REM Visual Studio creates a "obj" sub-folder. Since there is no way to disable it - just delete it.
rd "%~dp0obj" /S /Q

ENDLOCAL
EXIT /B

:SHOWHELP
TITLE "%~nx0 %1"
ECHO. & ECHO.
ECHO Usage:   %~nx0 [32^|64] [Clean^|Build^|Rebuild] [Debug] [VS2010^|VS2012]
ECHO.
ECHO Notes:   The arguments are not case sensitive.
ECHO. & ECHO.
ECHO Executing "%~nx0" will use the defaults: "%~nx0"
ECHO.
ENDLOCAL
EXIT /B
