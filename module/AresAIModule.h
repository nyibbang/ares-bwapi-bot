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

#include <BWAPI.h>

class AresAIModule : public BWAPI::AIModule
{
    public:
        virtual void onStart();
        virtual void onEnd(bool isWinner);
        virtual void onSaveGame(std::string gameName);
        virtual void onFrame();
        virtual void onSendText(std::string text);
        virtual void onReceiveText(BWAPI::Player player, std::string text);
        virtual void onPlayerLeft(BWAPI::Player player);
        virtual void onNukeDetect(BWAPI::Position target);
        virtual void onUnitDiscover(BWAPI::Unit unit);
        virtual void onUnitEvade(BWAPI::Unit unit);
        virtual void onUnitShow(BWAPI::Unit unit);
        virtual void onUnitHide(BWAPI::Unit unit);
        virtual void onUnitCreate(BWAPI::Unit unit);
        virtual void onUnitDestroy(BWAPI::Unit unit);
        virtual void onUnitMorph(BWAPI::Unit unit);
        virtual void onUnitRenegade(BWAPI::Unit unit);
        virtual void onUnitComplete(BWAPI::Unit unit);
};

