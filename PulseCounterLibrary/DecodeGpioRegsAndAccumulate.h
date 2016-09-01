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

#include "ConfiguredChannels.h"

#include <boost/thread/mutex.hpp>

#include <vector>

class DecodeGpioRegsAndAccumulate
{
public:
	DecodeGpioRegsAndAccumulate(const ConfiguredChannels &configured_channels);

	//
	// Description: Process the value read from the GPIO register representing all gpio pins. 
	//              Look at the value (high or low) for each configured pin (1 bit in gpio registry value) and perform accumulate as required.
	//
	bool process(unsigned int piGpioReg);

	const std::vector<unsigned int>& getResult();
	
	unsigned int getPinValue(unsigned int regValue, unsigned int pin);



private:
	const ConfiguredChannels&  m_configured_channels;

	std::vector<unsigned int> m_gpioPinAccumulation;
	std::vector<unsigned int> m_gpioPinAccumulationLocal;
	std::vector<unsigned int> m_lastGpioValue;

	boost::mutex		 m_accumulate_mutex;

};

