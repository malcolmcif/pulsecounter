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

#include "PulseCounterMain.h"

#include "PollAndAccumulateRunner.h"
#include "DisplayData.h"
#include "SummaryToFile.h"

#include <boost/thread.hpp>
#include <boost/foreach.hpp>

#include <iostream>

using namespace std;

PulseCounterMain::PulseCounterMain(RPiInterface &rpi) : m_rpi(rpi)
{
}

void PulseCounterMain::run(const PulseCounterConfiguration &readAllPollConfig, ConfiguredChannels& cc)
{
	if (m_rpi.wiringPiSetupPhys() < 0)
	{
		cerr << "Unable to setup wiringPi: " << strerror(errno) << endl;
		return;
	}

	printf("Configured with multi thread display\n");

	cerr << readAllPollConfig.displayPullUpDownState() << endl;

	BOOST_FOREACH(int activePin, readAllPollConfig.activePins)
	{
		cc.addChannel(activePin);
		m_rpi.pullUpDnControl(activePin, readAllPollConfig.pullUpDownState);
	}

	if (readAllPollConfig.duration == 0)
		printf("sample forever\n");
	else
		printf("sample for %d seconds\n", readAllPollConfig.duration);

	bool process(true);

	PollAndAccumulateRunner pollAndAccummulateThread(cc, cc.getChannelsMask(), m_rpi, readAllPollConfig.cpuRestPeriod);

	boost::thread thrd(&PollAndAccumulateRunner::run, &pollAndAccummulateThread);

	const unsigned int TIMING_TOLERANCE = 5;	// 5 milliseconds

	unsigned int start, current, previous, previous_display;
	start = current = previous = previous_display = m_rpi.millis();
	DisplayData display(readAllPollConfig, readAllPollConfig.statusonly);
	display.header();
	SummaryToFile summaryToFile(display, readAllPollConfig.statusfname);

	while (process)
	{
		boost::this_thread::sleep(boost::posix_time::seconds(1));

		current = m_rpi.millis();

		if (readAllPollConfig.duration > 0)
			if ((current - start + TIMING_TOLERANCE) >= (readAllPollConfig.duration * 1000))
				process = false;

		if ((current - previous_display + TIMING_TOLERANCE) >= 1000)
		{
			display.channels(pollAndAccummulateThread.getResult());
			summaryToFile.dumpDataToFile(pollAndAccummulateThread.getResult(), current - start);
			previous_display = current;
		}
	}

	display.channels(pollAndAccummulateThread.getResult());
	display.summary(pollAndAccummulateThread.getResult(), current - start);

	pollAndAccummulateThread.stop();
	thrd.join();
}
