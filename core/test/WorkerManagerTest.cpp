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
#include "MockDispatcher.h"
#include "MockCommander.h"
#include <gtest/gtest.h>

using namespace ares;
using namespace ::testing;

TEST(WorkerManager, WorkerManager_sends_idle_workers_to_harvest_minerals)
{
    test::MockDispatcher<abc::WorkerEventListener> dispatcher;
    test::MockCommander commander;
    EXPECT_CALL(dispatcher, suscribe(_));
    WorkerManager manager(dispatcher, commander);
    const int unitId = 42;
    EXPECT_CALL(commander, execute(CommandType::HarvestClosestMineral, unitId));
    manager.onWorkerIdle(unitId);
    EXPECT_CALL(dispatcher, unsuscribe(Ref(manager)));
}
