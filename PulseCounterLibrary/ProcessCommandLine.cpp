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

#include "ProcessCommandLine.h"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include <iostream>

namespace
{
	void configureChannels(PulseCounterConfiguration &readAllPollConfig)
	{
		if (readAllPollConfig.activePins.size() == 0)
		{
			readAllPollConfig.activePins.push_back(11);
			readAllPollConfig.activePins.push_back(12);
			readAllPollConfig.activePins.push_back(13);
			readAllPollConfig.activePins.push_back(15);
			readAllPollConfig.activePins.push_back(16);
			readAllPollConfig.activePins.push_back(18);
			readAllPollConfig.activePins.push_back(22);
			readAllPollConfig.activePins.push_back(36);
			readAllPollConfig.activePins.push_back(38);
			readAllPollConfig.activePins.push_back(19);
			readAllPollConfig.activePins.push_back(21);
			readAllPollConfig.activePins.push_back(23);
		}
	}
}

using namespace std;

void ProcessCommandLine::usage(const boost::program_options::options_description &desc)
{
	cout << desc << endl;
}

bool ProcessCommandLine::parse(int argc, char* argv[], PulseCounterConfiguration &readAllPollConfig)
{
	const char* ACTIVE_PINS_ARG		= "activepins";
	const char* CPU_REST_PERIOD_ARG = "cpuRestPeriod";
	const char* DEBUG_ARG			= "debug";
	const char* DURATION_ARG		= "duration";
	const char* PULLUPSTATE_ARG		= "pullupstate";
	const char* STATUS_FNAME_ARG	= "statusfname";
	const char* STATUS_ONLY_ARG		= "statusonly";

	boost::program_options::variables_map vm;

	std::map <std::string, int> pinPullUpState;
	std::string activePins;
	int         pullupstate(-1);

	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "produce help message")
		(ACTIVE_PINS_ARG, boost::program_options::value<string>(&activePins)->required(), "--activepins <default> or <pinno,pinno,pinno...> eg. 11,12,13,15,16,18,22,36,38,19,21,23")
		(CPU_REST_PERIOD_ARG, boost::program_options::value<int>(&readAllPollConfig.cpuRestPeriod), "--cpuRestPeriod n , number of microseconds to sleep before reading gpio pins status, default is 300")
		(DEBUG_ARG, boost::program_options::value<string>(), "--debug 0-command trace, 1-debug, 2-info, 3-warning, 4-error, 5-fatal, + indicates all levels above as well eg. 3+")
		(DURATION_ARG, boost::program_options::value<unsigned int>(&readAllPollConfig.duration), "--duration n , number of seconds to measure before exiting, default is 0 (forever)")
		(PULLUPSTATE_ARG, boost::program_options::value<int>(&pullupstate), "--pullupstate 0-off (default), 1-down, 2-up, 3-do not configure")
		(STATUS_FNAME_ARG, boost::program_options::value<string>(&readAllPollConfig.statusfname), "--statusfname <status file> eg. /tmp/status.txt")
		(STATUS_ONLY_ARG, "--statusonly , do not display results to stdout, only the status file")
		;

	try
	{
		boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);

		if (vm.count("help"))
		{
			cout << desc << "\n";
			return false;
		}

		if (vm.count(STATUS_ONLY_ARG))
		{
			readAllPollConfig.statusonly = true;
		}

		if (vm.count(DEBUG_ARG))
		{
			std::string debugLevel = vm[DEBUG_ARG].as<string>();
			int level = boost::lexical_cast<int>(debugLevel[0]);

			if (debugLevel.size() > 1)
			{
				if (debugLevel[1] == '+')
				{
					boost::log::core::get()->set_filter(boost::log::trivial::severity >= level ||
						boost::log::trivial::severity == boost::log::trivial::fatal);
				}
				else if (debugLevel[1] == '-')
				{
					boost::log::core::get()->set_filter(boost::log::trivial::severity <= level ||
						boost::log::trivial::severity == boost::log::trivial::fatal);
				}
			}
			else
			{
				boost::log::core::get()->set_filter(boost::log::trivial::severity == level ||
					boost::log::trivial::severity == boost::log::trivial::fatal);
			}

			readAllPollConfig.showDebug = true;
		}

		boost::program_options::notify(vm);

		if (!activePins.empty())
		{
			stringstream ss(activePins);
			vector<string> result;

			while (ss.good())
			{
				string substr;
				getline(ss, substr, ',');
				if (substr == "default")
				{
					readAllPollConfig.activePins.clear();
					configureChannels(readAllPollConfig);
					break;
				}
				readAllPollConfig.activePins.push_back(boost::lexical_cast<int>(substr));
			}
		}

		if (pullupstate >= 0)
			switch (pullupstate)
		{
			case PulseCounterConfiguration::PullUpState_Off:
				readAllPollConfig.pullUpDownState = PulseCounterConfiguration::PullUpState_Off;
				break;
			case PulseCounterConfiguration::PullUpState_Down:
				readAllPollConfig.pullUpDownState = PulseCounterConfiguration::PullUpState_Down;
				break;
			case PulseCounterConfiguration::PullUpState_Up:
				readAllPollConfig.pullUpDownState = PulseCounterConfiguration::PullUpState_Up;
				break;
			case PulseCounterConfiguration::PullUpState_NotInitialised:
				readAllPollConfig.pullUpDownState = PulseCounterConfiguration::PullUpState_NotInitialised;
				break;
			default:
				cout << "invalid value passed to parameter --" << PULLUPSTATE_ARG << endl;
				usage(desc);
				return false;
		};

		readAllPollConfig.displayValues();
	}
	catch (boost::program_options::unknown_option &e)
	{
		cerr << "invalid parameter passed: " << e.get_option_name() << endl;
		usage(desc);
		return false;
	}
	catch (std::exception& e)
	{
		std::cerr << "Error: " << e.what() << "\n";
		usage(desc);
		return false;
	}
	catch (...)
	{
		std::cerr << "Unknown error!" << "\n";
		usage(desc);
		return false;
	}

	return true;
}
