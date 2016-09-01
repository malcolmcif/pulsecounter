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
@set OLDPATH=%PATH%
call ..\BoostBuildEnvironment.bat

b2 link=static toolset=msvc-12 threading=multi release
@set PATH=%OLDPATH%
