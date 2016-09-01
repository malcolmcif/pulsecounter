/*
 *	Copyright (c) 2016 Malcolm Cifuentes
 ***********************************************************************
 * This file is part of PulseCounter:
 *	https://https://github.com/pulsecounter/
 *
 *    PulseCounter is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as
 *    published by the Free Software Foundation, either version 3 of the
 *    License, or (at your option) any later version.
 *
 *    PulseCounter is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with PulseCounter.
 *    If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#ifndef WIN32
#include "RPiInterfaceReal.h"

#include <wiringPi.h>

int  RPiInterfaceReal::piHiPri(int pri)
{
	return ::piHiPri(pri);
}

unsigned int  RPiInterfaceReal::digitalReadAll(unsigned short inputReg)
{
    // this function was added the wiring pi library for this application
	return ::digitalReadAll(inputReg);
}

void RPiInterfaceReal::delay(unsigned int howLong)
{
	::delay(howLong);
}


void RPiInterfaceReal::delayMicroSeconds(unsigned int howLong)
{
    ::delayMicroseconds(howLong);
}

void RPiInterfaceReal::pinMode(int pin, int mode)
{
	::pinMode(pin, mode);
}

void RPiInterfaceReal::pullUpDnControl(int pin, int pud)
{
	::pullUpDnControl(pin, pud);
}

int  RPiInterfaceReal::wpiPinToGpio(int wPiPin)
{
	return ::wpiPinToGpio(wPiPin);
}

int  
RPiInterfaceReal::physPinToGpio(int physPin)
{
	return ::physPinToGpio(physPin);
}

int  RPiInterfaceReal::wiringPiSetupPhys()
{
	return ::wiringPiSetupPhys();
}

unsigned int RPiInterfaceReal::millis()
{
	return ::millis();
}

// private:
RPiInterfaceReal::RPiInterfaceReal()
{
}
#endif
