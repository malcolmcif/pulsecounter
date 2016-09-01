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

//
// Description: Tests for some parts of the system, if you need more add them:)
//
#include <PulseCounterLibrary/PulseCounterMain.h>
#include <PulseCounterLibrary/DecodeGpioRegsAndAccumulate.h>
#include <PulseCounterLibrary/ProcessCommandLine.h>
#include <PulseCounterLibrary/RPiInterfaceEmulated.h>

#include <boost/test/unit_test.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/assign/std/vector.hpp>
#include <boost/assign.hpp>
#include <boost/filesystem.hpp>
#include <boost/assign.hpp>
#include <boost/functional/hash.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/std/vector.hpp>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/utility/binary.hpp>

#include <bitset>

namespace
{
	const int EDGES			= 100;

	const int ACTIVE_PHYS_PINS[] = {11,12,13,15,16,18,22,36,38,19,21,23};			// physical pin number on gpio header 1 to 40
	const int ACTIVE_GPIO_PIN[] = { 17, 18, 27, 22, 23, 24, 25, 16, 20, 10, 9, 11 };	// physcial pin to gpio bit position in registry

	void configure12Channels(ConfiguredChannels &cc)
	{
		cc.addChannel(11);
		cc.addChannel(12);
		cc.addChannel(13);
		cc.addChannel(15);
		cc.addChannel(16);
		cc.addChannel(18);
		cc.addChannel(22);
		cc.addChannel(36);
		cc.addChannel(38);
		cc.addChannel(19);
		cc.addChannel(21);
		cc.addChannel(23);
	}

	void configure6Channels(ConfiguredChannels &cc)
	{
		cc.addChannel(11);
		cc.addChannel(12);
		cc.addChannel(13);
		cc.addChannel(15);
		cc.addChannel(16);
		cc.addChannel(18);
	}

	struct DisableBoostLoggingDuringTests
	{
		DisableBoostLoggingDuringTests()
		{ 
			boost::log::core::get()->set_filter(boost::log::trivial::severity == boost::log::trivial::fatal);
		}
	};
}

BOOST_GLOBAL_FIXTURE(DisableBoostLoggingDuringTests);

BOOST_AUTO_TEST_CASE(ProcessCommandLineTesterStandardParams)
{
	int argc(14);
	char *argv[] = { "exename", "--debug", "3+", "--pullupstate", "2", 
                     "--duration", "102", "--statusonly", 
                     "--statusfname", "/tmp/t.txt", 
                     "--activepins", "11,12,13,15,16,18,22,36,38,19,21,23",
					 "--cpuRestPeriod", "1"};

	PulseCounterConfiguration config;

	BOOST_CHECK(config.showDebug == false);
	BOOST_CHECK(config.pullUpDownState == PulseCounterConfiguration::PullUpState_Off);
	BOOST_CHECK(config.duration == 0);
	BOOST_CHECK(config.statusonly == false);
	BOOST_CHECK(config.statusfname == "/tmp/status.txt");
	BOOST_CHECK(config.activePins.size() == 0);
	BOOST_CHECK(config.cpuRestPeriod == 300);

	ProcessCommandLine pcl;
	BOOST_CHECK(pcl.parse(argc, argv, config) == true);
	BOOST_CHECK(config.showDebug == true);
	BOOST_CHECK(config.pullUpDownState == PulseCounterConfiguration::PullUpState_Up);
	BOOST_CHECK(config.duration == 102);
	BOOST_CHECK(config.statusonly == true);
	BOOST_CHECK(config.statusfname == "/tmp/t.txt");
	BOOST_CHECK(config.activePins.size() == 12);
	BOOST_CHECK(config.cpuRestPeriod == 1);

	for (unsigned int i = 0; i < config.activePins.size(); i++)
		BOOST_CHECK(config.activePins[i] == ACTIVE_PHYS_PINS[i]);
}

BOOST_AUTO_TEST_CASE(ProcessCommandLineTesterDefaultPins)
{
	int argc(3);
	char *argv[] = { "exename", "--activepins", "default" };
	PulseCounterConfiguration config;

	BOOST_CHECK(config.activePins.size() == 0);

	ProcessCommandLine pcl;
	BOOST_CHECK(pcl.parse(argc, argv, config) == true);
	BOOST_CHECK(config.activePins.size() == 12);
	for (unsigned int i = 0; i < config.activePins.size(); i++)
		BOOST_CHECK(config.activePins[i] == ACTIVE_PHYS_PINS[i]);
}

BOOST_AUTO_TEST_CASE(ConfiguredChannelTester)
{
	RPiInterfaceRegistry::createEmulatedOnly();
	RPiInterface &rpi = RPiInterfaceRegistry::get();

	ConfiguredChannels cc(rpi);

	configure12Channels(cc);

	for (unsigned int pin = 1; pin <= cc.totalChannels(); pin++)
	{
		BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << "cc.getChannelPin(" << pin << ") = " << cc.getChannelPin(pin);
		BOOST_CHECK(cc.getChannelPin(pin) == ACTIVE_GPIO_PIN[pin-1]);
	}

	BOOST_CHECK(BOOST_BINARY(0000 1011 1101 0111 0000 1110 0000 0000) == cc.getChannelsMask());
}

//
// Description: toggle all configured pins high and low, accumulate each transition
//
BOOST_AUTO_TEST_CASE(DecodeGpioRegsAndAccumulateBasicTester)
{
	const unsigned int ONLY_PIN_BITS_SET = 0x0bd70e00;

	RPiInterfaceRegistry::createEmulatedOnly();
	RPiInterface &rpi = RPiInterfaceRegistry::get();

	ConfiguredChannels cc(rpi);
	configure12Channels(cc);
	DecodeGpioRegsAndAccumulate decoder(cc);

	for (int i = 0; i < EDGES; i++)
	{
		decoder.process(0);
		decoder.process(ONLY_PIN_BITS_SET & cc.getChannelsMask());
	}

	BOOST_FOREACH(int channel, decoder.getResult())
	{
		BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << "channel = " << channel;
		BOOST_CHECK(channel == EDGES);
	}
}

//
// Description: toggle all configured pins high and low, accumulate each transition
//
BOOST_AUTO_TEST_CASE(DecodeGpioRegsAndAccumulateSixChannelTester)
{
	const unsigned int ONLY_PIN_BITS_SET = 0x9c60000;

	RPiInterfaceRegistry::createEmulatedOnly();
	RPiInterface &rpi = RPiInterfaceRegistry::get();

	ConfiguredChannels cc(rpi);

	configure6Channels(cc);

	DecodeGpioRegsAndAccumulate decoder(cc);

	for (int i = 0; i < EDGES; i++)
	{
		decoder.process(0);
		decoder.process(ONLY_PIN_BITS_SET & cc.getChannelsMask());
	}

	BOOST_FOREACH(int channel, decoder.getResult())
	{
		BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << "channel = " << channel;
		BOOST_CHECK(channel == EDGES);
	}
}

//
// Description: read data from the pi interface
//
BOOST_AUTO_TEST_CASE(PiInterfaceTester)
{
	RPiInterfaceRegistry::createEmulatedOnly();
	RPiInterface &rpi = RPiInterfaceRegistry::get();

	BOOST_CHECK(rpi.digitalReadAll(0) == 0xFFFFFFFF);
	BOOST_CHECK(rpi.digitalReadAll(0) == 0);
	BOOST_CHECK(rpi.digitalReadAll(0) == 0xFFFFFFFF);
	BOOST_CHECK(rpi.digitalReadAll(0) == 0);
}

//
// Description: set each gpio pin high in sequence and verify that the correct pin is high and all others are low
//
BOOST_AUTO_TEST_CASE(DecodeGpioRegsGetPinValueTester)
{
	RPiInterfaceRegistry::createEmulatedOnly();
	RPiInterface &rpi = RPiInterfaceRegistry::get();

	ConfiguredChannels cc(rpi);
	configure12Channels(cc);

	DecodeGpioRegsAndAccumulate decoder(cc);

	for (unsigned int channel = 1; channel < cc.totalChannels(); channel++)
	{
		unsigned int regValue = 1 << cc.getChannelPin(channel);
		BOOST_LOG_TRIVIAL(debug) << __FUNCTION__ << "channel = " << channel << ", regval = " << std::bitset<32>(regValue);

		BOOST_CHECK(decoder.getPinValue(regValue, cc.getChannelPin(channel)) == 1);

		for (unsigned int altchannel = 1; altchannel < cc.totalChannels(); altchannel++)
		{
			if (channel != altchannel)
				BOOST_CHECK(decoder.getPinValue(regValue, cc.getChannelPin(altchannel)) == 0);
		}
	}
}

//
// Description: this test is really only testing the ability of the main line to run and exit which is sufficient for the moment.
//
BOOST_AUTO_TEST_CASE(PulseCounterIntegrateMainMustExit)
{
	int argc(6);
	char *argv[] = { "exename", "--activepins", "default",
                                "--duration", "1", "--statusonly" };

	RPiInterfaceRegistry::createEmulatedOnly();
	RPiInterface &rpi = RPiInterfaceRegistry::get();

	ConfiguredChannels cc(rpi);
	PulseCounterConfiguration config;

	BOOST_CHECK(config.activePins.size() == 0);

	ProcessCommandLine pcl;
	BOOST_CHECK(pcl.parse(argc, argv, config) == true);

	PulseCounterMain main(rpi);
	main.run(config, cc);
}
