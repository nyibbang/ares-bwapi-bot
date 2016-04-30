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

#pragma once

#include "WorkerEventListener.h"

namespace ares
{
namespace core
{

namespace abc
{
template <class> class Dispatcher;
class Commander;
}

class ResourcesHarvester final : public abc::WorkerEventListener
{
    public:
        ResourcesHarvester(abc::Dispatcher<abc::WorkerEventListener>& dispatcher,
                      abc::Commander& commander);
        ~ResourcesHarvester();

        void onWorkerIdle(int unitId) override;

    private:
        abc::Dispatcher<abc::WorkerEventListener>& m_dispatcher;
        abc::Commander& m_commander;
};

}
}
