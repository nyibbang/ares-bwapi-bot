/*
 * AresBWAPIBot : A bot for SC:BW using BWAPI library;
 * Copyright (C) 2015 Vincent PALANCHER; Florian LEMEASLE
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#include "WorkerManager.h"
#include "abc/Dispatcher.h"
#include "abc/Commander.h"
#include "trace/Trace.h"

namespace ares
{

ResourcesHarvester::ResourcesHarvester(abc::Dispatcher<abc::WorkerEventListener>& dispatcher, abc::Commander& commander)
    : m_dispatcher(dispatcher)
    , m_commander(commander)
{
    m_dispatcher.suscribe(*this);
}

ResourcesHarvester::~ResourcesHarvester()
{
    m_dispatcher.unsuscribe(*this);
}

void ResourcesHarvester::onWorkerIdle(int unitId)
{
    m_commander.execute(CommandType::HarvestClosestMineral, unitId);
    ARES_DEBUG() << "Sent idle worker (ID: " << unitId << ") back to minerals harvesting";
}

}
