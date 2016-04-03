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

#include "core/abc/Commander.h"
#include "core/abc/Dispatcher.h"
#include <BWAPI.h>
#include <list>
#include <memory>

namespace ares
{

namespace core
{
namespace abc
{
class GameEventListener;
class WorkerEventListener;
}
class ResourcesHarvester;
}

namespace module
{

class Commander final : public ares::core::abc::Commander
{
    private:
        void execute(ares::core::CommandType type, int unitId) override;
};

class Module final : public BWAPI::AIModule
                   , public core::abc::Dispatcher<core::abc::GameEventListener>
                   , public core::abc::Dispatcher<core::abc::WorkerEventListener>
{
    public:
        Module();

        void onStart() override;
        void onEnd(bool isWinner) override;
        void onSaveGame(std::string gameName) override;
        void onFrame() override;
        void onSendText(std::string text) override;
        void onReceiveText(BWAPI::Player player, std::string text) override;
        void onPlayerLeft(BWAPI::Player player) override;
        void onNukeDetect(BWAPI::Position target) override;
        void onUnitCreate(BWAPI::Unit unit) override;
        void onUnitMorph(BWAPI::Unit unit) override;

        void suscribe(core::abc::GameEventListener& listener) override;
        void unsuscribe(core::abc::GameEventListener& listener) override;

        void suscribe(core::abc::WorkerEventListener& listener) override;
        void unsuscribe(core::abc::WorkerEventListener& listener) override;

    private:
        std::unique_ptr<Commander> m_commander;
        std::unique_ptr<core::ResourcesHarvester> m_rscHvst;
        template <class Type> using RefList = std::list<std::reference_wrapper<Type>>;
        RefList<core::abc::GameEventListener> m_gameListeners;
        RefList<core::abc::WorkerEventListener> m_workerListeners;
};

}
}
