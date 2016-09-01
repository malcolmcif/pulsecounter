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

#include "RPiInterface.h"

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/console.hpp>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

#include <boost/thread/thread.hpp>

#include <vector>
#include <algorithm>
#include <iterator>
#include <map>
#include <cassert>

#ifndef WIN32
#include <wiringPi.h>
#include "rawgpio.h"
#include <sys/mman.h>
#else
//#include "wiringPiWindows.h"
#endif

#include <string>

using namespace std;

class ReadAllPollConfiguration
{
public:

    ReadAllPollConfiguration() :	showDebug(false), duration(0), pullUpDownState(0), 
									statusonly(false), statusfname("/tmp/status.txt"), activechannels(6) {}
    bool showDebug;
    unsigned int  duration;
    int pullUpDownState;
    bool statusonly;
    std::string statusfname;
    int activechannels;

    void displayValues()
    {
        BOOST_LOG_TRIVIAL(debug) << "ReadAllPollConfiguration: showDebug = " << showDebug;
        BOOST_LOG_TRIVIAL(debug) << "ReadAllPollConfiguration: "			 << displayPullUpDownState();
        BOOST_LOG_TRIVIAL(debug) << "ReadAllPollConfiguration: pullUpDownState = " << pullUpDownState;
        BOOST_LOG_TRIVIAL(debug) << "ReadAllPollConfiguration: statusonly      = " << statusonly;
        BOOST_LOG_TRIVIAL(debug) << "ReadAllPollConfiguration: statusfname     = " << statusfname;
        BOOST_LOG_TRIVIAL(debug) << "ReadAllPollConfiguration: activechannels  = " << activechannels;
    }

    std::string displayPullUpDownState() const
    {
        switch (pullUpDownState)
        {
		case RPiInterface::PullUpState_Off:
            return "pullUpDownState = Unused pin internal resistors configured to tristate(off or float)";
		case RPiInterface::PullUpState_Down:
            return "pullUpDownState = Unused pin internal resistors configured to pulldown";
			case RPiInterface::PullUpState_Up:
            return "pullUpDownState = Unused pin internal resistors configured to pullup";
        default:
            assert(0 && "logic bug");
        }
        return "logic bug - should never happen";
    }
};

class DecodeGpioRegsAndAccumulate
{
public:
	DecodeGpioRegsAndAccumulate(const std::vector<int> &gpioPhysPinMap) : m_gpioPhysPinMap(gpioPhysPinMap)
	{
		m_gpioPinAccumulation.resize(12, 0);
		m_lastGpioValue.resize(12, 0);
		m_gpioPinAccumulationLocal.resize(12, 0);
	}

	bool run(unsigned int piGpioReg)
	{
		bool changesDetected(false);

		unsigned int currentgpioValue(0);

		boost::lock_guard<boost::mutex> lockGuard(m_accumulate_mutex);

		for (unsigned int i = 0; i < m_gpioPhysPinMap.size(); i++)
		{
			currentgpioValue = getPinValue(piGpioReg, m_gpioPhysPinMap[i]);
			if (currentgpioValue != m_lastGpioValue[i])
			{
				if (currentgpioValue == 1) // only count rising edge
				{
					m_gpioPinAccumulation[i] += 1;
					changesDetected = true;
				}

				m_lastGpioValue[i] = currentgpioValue;
			}
		}

		return changesDetected;
	}

	unsigned int getPinValue(unsigned int regValue, unsigned int pin)
	{
		return ((regValue & (1 << (pin & 31))) && 1);
	}

	const std::vector<unsigned int>& getResult()
	{
		m_gpioPinAccumulationLocal.clear();
		boost::lock_guard<boost::mutex> lockGuard(m_accumulate_mutex);

		copy(m_gpioPinAccumulation.begin(), m_gpioPinAccumulation.end(), back_inserter(m_gpioPinAccumulationLocal));

		return m_gpioPinAccumulationLocal;
	}

private:
	const std::vector<int>&   m_gpioPhysPinMap;
	std::vector<unsigned int> m_gpioPinAccumulation;
	std::vector<unsigned int> m_gpioPinAccumulationLocal;
	std::vector<unsigned int> m_lastGpioValue;

	boost::mutex			  m_accumulate_mutex;

};

class PollAndAccumulateRunner
{
public:
	PollAndAccumulateRunner(const std::vector<int> &gpioPhysPinMap, unsigned int gpio_channel_mask) : m_decodeAndAccummulate(gpioPhysPinMap), 
																									  m_keepThreadRunning(true),
																									  m_gpio_channel_mask(gpio_channel_mask)
	{}

	void run()
	{
		int ret = RPiInterface::piHiPri(90);

		BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << "set thread priority to 90, status = " << ret << std::endl;

		unsigned int last = RPiInterface::digitalReadAll(0) & m_gpio_channel_mask;

		while (m_keepThreadRunning)
		{
			unsigned int v = RPiInterface::digitalReadAll(0) & m_gpio_channel_mask;

			if (v != last)
			{
				m_decodeAndAccummulate.run(v);
				last = v;
			}
			RPiInterface::delay(5);
		}
		BOOST_LOG_TRIVIAL(info) << __FUNCTION__ << "exit" << endl;
	}

	void stop()
	{
		m_keepThreadRunning = false;
	}
					
	const std::vector<unsigned int>& getResult()
	{
		return m_decodeAndAccummulate.getResult();
	}

private:
	DecodeGpioRegsAndAccumulate m_decodeAndAccummulate;
	bool						m_keepThreadRunning;
	const unsigned int          m_gpio_channel_mask;
};

class DisplayData
{
public:
	DisplayData(bool nodisplay = false) : m_nodisplay(nodisplay) {}

	void header(FILE *stream = stdout)
	{
		if (m_nodisplay && stream == stdout)
			return;

		fprintf(stream, "Signal Counts for Each Pin\n");
		fprintf(stream, "%6d %6d %6d %6d %6d %6d %6d %6d %6d %6d %6d %6d\n", 0, 1, 2, 3, 4, 5, 6, 27, 28, 12, 13, 14);
	}

	void channels(const vector<unsigned int>& gpioPinAccumulation, FILE *stream = stdout)
	{
		if (m_nodisplay && stream == stdout)
			return;

		for (unsigned int i = 0; i<gpioPinAccumulation.size(); i++)
			fprintf(stream,"%6d ", gpioPinAccumulation[i]);

		fprintf(stream,"\r");
		fflush(stream);
	}

	void summary(const vector<unsigned int>& gpioPinAccumulation, unsigned int duration, FILE *stream = stdout)
	{
		if (m_nodisplay && stream == stdout)
			return;

		MinMaxT minmax = findMinMax(gpioPinAccumulation);

		unsigned long sum = static_cast<unsigned long>(std::accumulate(gpioPinAccumulation.begin(), gpioPinAccumulation.end(), 0.0));
		double mean = 0;

		if (minmax._total>0)
			mean = sum / minmax._total;

		fprintf(stream, "\nStatics -\n");
		fprintf(stream, "   duration - %.2f\n", (duration / 1000.0));
		fprintf(stream, "   fs       - %.2f\n", (minmax._max) / (duration / 1000.0));
		fprintf(stream, "   mean     - %.0f\n", mean);
		fprintf(stream, "   min      - %d\n", minmax._min);
		fprintf(stream, "   max      - %d\n", minmax._max);
		fprintf(stream, "   delta    - %d\n", minmax._max - minmax._min);
	}

private:
	struct MinMaxT
	{
		MinMaxT() : _min(0), _max(0), _total(0) {}
		unsigned int _min;
		unsigned int _max;
		unsigned int _total;
	};

	MinMaxT findMinMax(std::vector<unsigned int> a)
	{
		MinMaxT m;
		m._min = m._max = a[0];

		for (size_t i = 1; i < a.size(); i++)
		{
			if (a[i] == 0)
				continue;
			else
			{
				m._total++;
				if (m._min > a[i])
					m._min = a[i];

				if (m._max < a[i])
					m._max = a[i];
			}
		}

		return m;
	}

	bool m_nodisplay;
};

class SummaryToFile
{
public:
	SummaryToFile(DisplayData &dd, std::string ofname) : m_display_data(dd), m_fname(ofname)
	{
	}

	void dumpDataToFile(const std::vector<unsigned int>& result, unsigned int duration)
	{
		FILE * pFile;
		pFile = fopen(m_fname.c_str(), "w");
		if (pFile != NULL)
		{
			m_display_data.header(pFile);
			m_display_data.channels(result,pFile);
			m_display_data.summary(result,duration,pFile);
			fclose(pFile);
		}
	}

private:
	DisplayData m_display_data;
	std::string m_fname;
};

