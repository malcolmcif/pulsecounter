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

#include "PollAndAccumulateRunner.h"

#include "DecodeGpioRegsAndAccumulate.h"

#include <boost/log/trivial.hpp>

using namespace std;

PollAndAccumulateRunner::PollAndAccumulateRunner(const ConfiguredChannels &configured_channels, 
												 unsigned int gpio_channel_mask, 
												 RPiInterface &rpi, 
												 int cpu_rest_period) :
															m_decodeAndAccummulate(configured_channels),
															m_keepThreadRunning(true),
															m_gpio_channel_mask(gpio_channel_mask),
															m_rpi(rpi),
															m_cpu_rest_period(cpu_rest_period)
{}

void PollAndAccumulateRunner::run()
{
	int ret = m_rpi.piHiPri(90);

	BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << "set thread priority to 90, status = " << ret;

	unsigned int last = m_rpi.digitalReadAll(0) & m_gpio_channel_mask;

	while (m_keepThreadRunning)
	{
		unsigned int v = m_rpi.digitalReadAll(0) & m_gpio_channel_mask;
		BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << " read bits = " << hex << v << dec;

		if (v != last)
		{
			m_decodeAndAccummulate.process(v);
			last = v;
		}
		else if (m_cpu_rest_period > -1)
			m_rpi.delayMicroSeconds(m_cpu_rest_period);
	}
	BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << "exit" << endl;
}

void PollAndAccumulateRunner::stop()
{
	m_keepThreadRunning = false;
}

const std::vector<unsigned int>& PollAndAccumulateRunner::getResult()
{
	return m_decodeAndAccummulate.getResult();
}

