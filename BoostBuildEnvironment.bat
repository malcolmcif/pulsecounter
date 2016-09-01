@ECHO OFF
REM	Copyright (c) 2016 Malcolm Cifuentes
REM **********************************************************************
REM This file is part of PulseCounter:
REM	https://https://github.com/pulsecounter/
REM
REM    PulseCounter is free software: you can redistribute it and/or modify
REM    it under the terms of the GNU Lesser General Public License as
REM    published by the Free Software Foundation, either version 3 of the
REM    License, or (at your option) any later version.
REM
REM    PulseCounter is distributed in the hope that it will be useful,
REM    but WITHOUT ANY WARRANTY; without even the implied warranty of
REM    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
REM    GNU Lesser General Public License for more details.
REM
REM    You should have received a copy of the GNU Lesser General Public
REM    License along with PulseCounter.
REM    If not, see <http://www.gnu.org/licenses/>.

@echo off

set OLD_PATH=%PATH%
set BOOST_ROOT=%~dp0\boost\boost-build-2014.10
set PATH=%BOOST_ROOT%\bin;%PATH%
set BOOST_BUILD_PATH=%BOOST_ROOT%\share\boost-build

IF "%1" == "-arm49" GOTO ARM_SPECIFIC_49

echo "Visual Studio and MinGw building configured"
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
goto DONE

:ARM_SPECIFIC_49

echo "raspberry pi building configured g++ 4.9"
rem set CYGWIN=nodosfilewarning
set PATH=%PATH%;C:\Windows\system32;C:\SysGCC\Raspberry\4.9.2\bin;C:\Users\Jo\boost-build-2014.10\bin;C:\Windows\system32;

:DONE
