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

#include "RPiInterfaceRegistry.h"

#ifndef WIN32
#include "RPiInterfaceReal.h"
#endif

#include "RPiInterfaceEmulated.h"

std::auto_ptr<RPiInterface> RPiInterfaceRegistry::m_interface;
bool RPiInterfaceRegistry::m_emulated = false;

RPiInterface& 
RPiInterfaceRegistry::get()
{
	if (m_interface.get() == 0)
	{
		if (m_emulated == false)
		{
#ifdef WIN32
			m_interface.reset(new RPiInterfaceEmulated);
#else
			m_interface.reset(new RPiInterfaceReal);
#endif
		}
		else
		{
			m_interface.reset(new RPiInterfaceEmulated);
		}
	}

	return *m_interface;
}

void 
RPiInterfaceRegistry::createEmulatedOnly()
{
	m_emulated = true;
}
