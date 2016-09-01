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

#include "DecodeGpioRegsAndAccumulate.h"

#include <boost/thread/thread.hpp>
#include <boost/log/trivial.hpp>

#include <bitset> 
#include <assert.h>


DecodeGpioRegsAndAccumulate::DecodeGpioRegsAndAccumulate(const ConfiguredChannels &configured_channels) : m_configured_channels(configured_channels)
{
	assert(configured_channels.totalChannels() && "required input channels not set configured");
	m_gpioPinAccumulation.resize(configured_channels.totalChannels(), 0);
	m_lastGpioValue.resize(configured_channels.totalChannels(), 0);
	m_gpioPinAccumulationLocal.resize(configured_channels.totalChannels(), 0);
}

//
// Description: Process the value read from the GPIO register representing all gpio pins. 
//              Look at the value (high or low) for each configured pin (1 bit in gpio registry value) and perform accumulate as required.
//
bool DecodeGpioRegsAndAccumulate::process(unsigned int piGpioReg)
{
	bool changesDetected(false);

	unsigned int currentgpioValue(0);

	boost::lock_guard<boost::mutex> lockGuard(m_accumulate_mutex);
	BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << " process reg value = " << std::bitset<32>(piGpioReg);

	for (unsigned int i = 0; i < m_configured_channels.totalChannels(); i++)
	{
		currentgpioValue = getPinValue(piGpioReg, m_configured_channels.getChannelPin(i+1));
		if (currentgpioValue != m_lastGpioValue[i])
		{
			if (currentgpioValue == 1) // only count high signal
			{
				m_gpioPinAccumulation[i] += 1;
				changesDetected = true;
			}

			m_lastGpioValue[i] = currentgpioValue;
		}
	}

	return changesDetected;
}

const std::vector<unsigned int>& DecodeGpioRegsAndAccumulate::getResult()
{
	m_gpioPinAccumulationLocal.clear();
	boost::lock_guard<boost::mutex> lockGuard(m_accumulate_mutex);

	copy(m_gpioPinAccumulation.begin(), m_gpioPinAccumulation.end(), back_inserter(m_gpioPinAccumulationLocal));

	return m_gpioPinAccumulationLocal;
}

unsigned int DecodeGpioRegsAndAccumulate::getPinValue(unsigned int regValue, unsigned int pin)
{
	unsigned int shifter = (1 << (pin & 31));
	return ((regValue & shifter) && 1);
}
