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

#include "PulseCounterConfiguration.h"

#include <boost/program_options.hpp>

class ProcessCommandLine
{
public:
	struct PullUpStateType
	{
		PullUpStateType(int const& val) : value(val) { }
		int value;
	};

	void usage(const boost::program_options::options_description &desc);

	bool parse(int argc, char* argv[], PulseCounterConfiguration &readAllPollConfig);
};
