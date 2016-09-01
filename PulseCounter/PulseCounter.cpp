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

#include <PulseCounterLibrary/PulseCounterMain.h>
#include <PulseCounterLibrary/RPiInterfaceRegistry.h>
#include <PulseCounterLibrary/ProcessCommandLine.h>


#include <boost/filesystem.hpp>
#include <boost/assign.hpp>
#include <boost/functional/hash.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/std/vector.hpp>

#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/console.hpp>


#include <string>

#include <cstdio>

#include <iostream>
#include <string>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#ifdef WIN32
#   include <windows.h> 
#endif

using namespace std;

namespace
{
	const std::string	VERSION		= "0.3";
}

void configureBoostLog()
{
	using namespace boost;
	namespace logging = boost::log;
	namespace src = boost::log::sources;
	namespace expr = boost::log::expressions;
	namespace sinks = boost::log::sinks;
	namespace attrs = boost::log::attributes;
	namespace keywords = boost::log::keywords;

	//Initialize logging to std::clog

	boost::log::add_console_log(std::cout);

	typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
	boost::shared_ptr< text_sink > sink = boost::make_shared< text_sink >();

	sink->locked_backend()->auto_flush(true);

	// Register the sink in the logging core
	logging::core::get()->add_sink(sink);

	logging::core::get()->set_filter
		(
		log::trivial::severity >= log::trivial::warning
		);
	logging::add_common_attributes();
}

int main(int argc, char* argv[])
{

	try
	{
		boost::log::add_common_attributes();
		boost::log::sources::severity_logger< boost::log::trivial::severity_level > lg;

		configureBoostLog();

		ProcessCommandLine	pcl;
		PulseCounterConfiguration readAllPollConfig;

		cerr << "-- VERSION: " << VERSION << endl;

		if (pcl.parse(argc, argv, readAllPollConfig) == false)
			return -1;

			{
				boost::log::core::get()->flush();
				boost::log::core::get()->remove_all_sinks();

				RPiInterface &rpi = RPiInterfaceRegistry::get();

				ConfiguredChannels	cc(rpi);

				cerr << "-- rest cpu between samples for (us) : " << readAllPollConfig.cpuRestPeriod << endl;

				PulseCounterMain PulseCounterMain(rpi);
				PulseCounterMain.run(readAllPollConfig, cc);
			}
		}
	catch (...)
	{
		BOOST_LOG_TRIVIAL(fatal) << "An unhandled exception was caught, program exiting!";
	}

	return 0;
}
