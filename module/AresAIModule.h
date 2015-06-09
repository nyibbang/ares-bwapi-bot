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

#include "AbstractEventDispatcher.h"
#include <BWAPI.h>
#include <list>

namespace ares
{

class AbstractGameEventListener;
class AbstractWorkerEventListener;

class AresAIModule final : public BWAPI::AIModule
                         , public AbstractDispatcher<AbstractGameEventListener>
                         , public AbstractDispatcher<AbstractWorkerEventListener>
{
    public:
        void onStart();
        void onEnd(bool isWinner);
        void onSaveGame(std::string gameName);
        void onFrame();
        void onSendText(std::string text);
        void onReceiveText(BWAPI::Player player, std::string text);
        void onPlayerLeft(BWAPI::Player player);
        void onNukeDetect(BWAPI::Position target);
        void onUnitCreate(BWAPI::Unit unit);
        void onUnitMorph(BWAPI::Unit unit);

        void suscribe(AbstractGameEventListener& listener) override;
        void unsuscribe(AbstractGameEventListener& listener) override;

        void suscribe(AbstractWorkerEventListener& listener) override;
        void unsuscribe(AbstractWorkerEventListener& listener) override;

    private:
        std::list<std::reference_wrapper<AbstractGameEventListener>> m_gameListeners;
        std::list<std::reference_wrapper<AbstractWorkerEventListener>> m_workerListeners;
};

}

