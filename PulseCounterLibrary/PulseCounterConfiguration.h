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

#include <string>
#include <vector>

class PulseCounterConfiguration
{
public:
	enum  PullUpState { PullUpState_Off, PullUpState_Down, PullUpState_Up, PullUpState_NotInitialised };

	bool				showDebug;
	unsigned int		duration;
	PullUpState			pullUpDownState;
	bool				statusonly;
	std::string			statusfname;
	std::vector<int>	activePins;
	int					cpuRestPeriod;

	PulseCounterConfiguration();

	void displayValues() const;
	std::string displayPullUpDownState() const;
};


