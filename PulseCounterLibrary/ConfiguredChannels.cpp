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

#include "ConfiguredChannels.h"

#include <boost/foreach.hpp>
#include <boost/log/trivial.hpp>
#include <bitset> 
#include <stdexcept>

ConfiguredChannels::ConfiguredChannels(RPiInterface &rpi) : m_rpi(rpi)
{}

void ConfiguredChannels::addChannel(int pin)
{
	int channel = 1;

	if (m_channel_pinbit.size() > 0)
	{
		channel = m_channel_pinbit.rbegin()->first + 1;
	}
	m_channel_pinbit[channel] = m_rpi.physPinToGpio(pin);
}

int ConfiguredChannels::getChannelPin(int channel) const
{
	if (m_channel_pinbit.count(channel) == 0)
		throw std::runtime_error("channel not found");

	return m_channel_pinbit.at(channel);
}

unsigned int ConfiguredChannels::totalChannels() const
{
	return m_channel_pinbit.size();
}

unsigned int ConfiguredChannels::getChannelsMask() const
{
	unsigned int mask = 0;
	BOOST_FOREACH(const channel_pinbit_t::value_type &i, m_channel_pinbit)
	{
		mask = mask | (unsigned int)(1 << (i.second));
	}

	BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << " channel mask = " << std::bitset<32>(mask) << std::endl;

	return mask;
}
