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

#include "PulseCounterConfiguration.h"

#include <boost/log/trivial.hpp>

PulseCounterConfiguration::PulseCounterConfiguration() : showDebug(false), duration(0), pullUpDownState(PullUpState_Off),
														 statusonly(false), statusfname("/tmp/status.txt"), 
														 cpuRestPeriod(300)	// us
{
}

void PulseCounterConfiguration::displayValues() const
{
	BOOST_LOG_TRIVIAL(debug) << "PulseCounterConfiguration: showDebug = " << showDebug;
	BOOST_LOG_TRIVIAL(debug) << "PulseCounterConfiguration: " << displayPullUpDownState();
	BOOST_LOG_TRIVIAL(debug) << "PulseCounterConfiguration: pullUpDownState = " << pullUpDownState;
	BOOST_LOG_TRIVIAL(debug) << "PulseCounterConfiguration: statusonly      = " << statusonly;
	BOOST_LOG_TRIVIAL(debug) << "PulseCounterConfiguration: statusfname     = " << statusfname;
	BOOST_LOG_TRIVIAL(debug) << "PulseCounterConfiguration: cpuRestPeriod   = " << cpuRestPeriod;

	for (unsigned int channel = 0; channel < activePins.size(); channel++)
	{
		BOOST_LOG_TRIVIAL(debug) << "PulseCounterConfiguration: active channel-pin " << channel << " = " << activePins.at(channel);
	}
}

std::string PulseCounterConfiguration::displayPullUpDownState() const
{
	switch (pullUpDownState)
	{
	case PullUpState_Off:
		return "pullUpDownState = pin internal resistor configured to tristate(off or float)";
	case PullUpState_Down:
		return "pullUpDownState = pin internal resistor configured to pulldown";
	case PullUpState_Up:
		return "pullUpDownState = pin internal resistor configured to pullup";
	case PullUpState_NotInitialised:
		return "pullUpDownState = pin internal resistor not initialised by this program";
	default:
		assert(0 && "logic bug");
	}
	return "should never happen";
}


