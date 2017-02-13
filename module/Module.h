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

#include <BWAPI/AIModule.h>
#include <forward_list>
#include <memory>

namespace ares
{
namespace module
{

class Module final : public BWAPI::AIModule
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

    private:
        void sendIdleWorkersToMinerals();
        void buildWorkers();
};

}
}
