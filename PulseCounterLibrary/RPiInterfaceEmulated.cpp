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

#include "RPiInterfaceEmulated.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp> 

namespace
{
	int toggle = 0;
}

int  RPiInterfaceEmulated::piHiPri(int pri)
{
	return 0;
}

unsigned int  RPiInterfaceEmulated::digitalReadAll(unsigned short inputReg)
{
	toggle = !toggle;

	if (toggle == 0)
		return 0;
	else
		return 0xFFFFFFFF;
}

void RPiInterfaceEmulated::delay(unsigned int howLong)
{
	boost::this_thread::sleep(boost::posix_time::milliseconds(howLong));
}


void RPiInterfaceEmulated::delayMicroSeconds(unsigned int)
{

	boost::this_thread::sleep(boost::posix_time::milliseconds(1));
}


void RPiInterfaceEmulated::pinMode(int pin, int mode)
{
}

void RPiInterfaceEmulated::pullUpDnControl(int pin, int pud)
{
}

int  RPiInterfaceEmulated::wpiPinToGpio(int wPiPin)
{
	return -1;
}

int  RPiInterfaceEmulated::physPinToGpio(int physPin)
{
	const int MAX_PHYS_ARR = 64;

	static int physToGpioR2[MAX_PHYS_ARR] =
	{
		-1,		// 0
		-1, -1,	// 1, 2
		2, -1,	// 3, 4
		3, -1,	// 5, 6
		4, 14,  // 7, 8
		-1, 15, // 9, 10
		17, 18, // 11, 12
		27, -1, // 13, 14
		22, 23, // 15, 16
		-1, 24, // 17, 18
		10, -1, // 19, 20
		9, 25,  // 21, 22
		11, 8,  // 23, 24
		-1, 7,	// 25, 26

		// B+

		0, 1,	// 27, 28
		5, -1,	// 29, 30
		6, 12,	// 31, 32
		13, -1,	// 33, 34
		19, 16,	// 35, 36
		26, 20,	// 37, 38
		-1, 21,	// 39, 40

		// the P5 connector on the Rev 2 boards:

		-1, -1,
		-1, -1,
		-1, -1,
		-1, -1,
		-1, -1,
		28, 29,
		30, 31,
		-1, -1,
		-1, -1,
		-1, -1,
		-1, -1,
	};

	return physToGpioR2[physPin & (MAX_PHYS_ARR-1)];
}


int  RPiInterfaceEmulated::wiringPiSetupPhys()
{
	return 0;
}

unsigned int RPiInterfaceEmulated::millis()
{
	return static_cast<unsigned int>(boost::posix_time::second_clock::local_time().time_of_day().total_milliseconds());
}

// private:
RPiInterfaceEmulated::RPiInterfaceEmulated()
{
}
