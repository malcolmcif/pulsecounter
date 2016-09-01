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

#include "DisplayData.h"

#include <boost/foreach.hpp>

#include <numeric>

struct DisplayData::MinMaxT
{
	MinMaxT() : _min(0), _max(0), _total(0) {}
	unsigned int _min;
	unsigned int _max;
	unsigned int _total;
};

DisplayData::DisplayData(const PulseCounterConfiguration &config, bool nodisplay) : m_config(config), m_nodisplay(nodisplay) {}

void DisplayData::header(FILE *stream)
{
	if (m_nodisplay && stream == stdout)
		return;

	fprintf(stream, "Signal Counts for Each Pin\n");

	BOOST_FOREACH(int activePin, m_config.activePins)
	{
		fprintf(stream, "%6d ", activePin);
	}
	fprintf(stream, "\n");
}

void DisplayData::channels(const std::vector<unsigned int>& gpioPinAccumulation, FILE *stream)
{
	if (m_nodisplay && stream == stdout)
		return;

	for (unsigned int i = 0; i<gpioPinAccumulation.size(); i++)
		fprintf(stream, "%6d ", gpioPinAccumulation[i]);

	fprintf(stream, "\r");
	fflush(stream);
}


void DisplayData::summary(const std::vector<unsigned int>& gpioPinAccumulation, unsigned int duration, FILE *stream)
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


DisplayData::MinMaxT DisplayData::findMinMax(std::vector<unsigned int> a)
{
	MinMaxT m;
	m._min = m._max = a[0];
	m._total = 1;

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
