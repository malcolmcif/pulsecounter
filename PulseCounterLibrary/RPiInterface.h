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

#pragma once

#include <map>

//
// Description: This interface hides anything raspberry pi specific.
//
class RPiInterface
{
  public:
	virtual int  piHiPri(int) = 0;
	virtual unsigned int  digitalReadAll(unsigned short inputReg) = 0;
	virtual void delay(unsigned int howLong) = 0;
	virtual void delayMicroSeconds(unsigned int howLong) = 0;
	virtual void pinMode(int, int) = 0;
	virtual void pullUpDnControl(int, int) = 0;
	virtual int  wpiPinToGpio(int) = 0;
	virtual int  physPinToGpio(int physPin) = 0;
	virtual int  wiringPiSetupPhys() = 0;
	virtual unsigned int millis() = 0;
};
