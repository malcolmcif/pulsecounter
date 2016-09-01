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

#include "RPiInterface.h"
#include "RPiInterfaceRegistry.h"

//
// Description: This is the emulated version of the class, used for testing independent of the Raspberry Pi hardware
//
class RPiInterfaceEmulated : public RPiInterface
{
  public:
	friend class RPiInterfaceRegistry; // this class can only be accessed/created via the registry

	virtual int  piHiPri(int);
	virtual unsigned int  digitalReadAll(unsigned short inputReg);
	virtual void delay(unsigned int howLong);
	virtual void delayMicroSeconds(unsigned int howLong);
	virtual void pinMode(int, int);
	virtual void pullUpDnControl(int, int);
	virtual int  wpiPinToGpio(int);
	virtual int  physPinToGpio(int physPin);
	virtual int  wiringPiSetupPhys();
	virtual unsigned int millis();

  private:
    RPiInterfaceEmulated();
};
