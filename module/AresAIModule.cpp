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

#include "AresAIModule.h"
#include "abc/GameEventListener.h"
#include "abc/WorkerEventListener.h"
#include <iostream>

#define NOTIFY_LISTENERS(func, ...) \
    for (abc::GameEventListener& listener : m_gameListeners) { \
        listener.func(__VA_ARGS__); \
    }

namespace ares
{

void AresAIModule::onStart()
{
    NOTIFY_LISTENERS(onStart)

    // Hello World!
    BWAPI::Broodwar->sendText("Hello world from AresBWAPIBot !");

    // Print the map name.
    BWAPI::Broodwar << "Map name is " << BWAPI::Broodwar->mapName() << "." << std::endl;

    // Set the command optimization level so that common commands can be grouped
    // and reduce the bot's APM (Actions Per Minute).
    BWAPI::Broodwar->setCommandOptimizationLevel(2);

    // Check if this is a replay
    if (BWAPI::Broodwar->isReplay())
    {
        // Announce the players in the replay
        BWAPI::Broodwar << "Players in this replay:" << std::endl;

        // Iterate all the players in the game using a std:: iterator
        for(auto p : BWAPI::Broodwar->getPlayers())
        {
            // Only print the player if they are not an observer
            if (!p->isObserver()) {
                BWAPI::Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
            }
        }

    }
    else // if this is not a replay
    {
        // Retrieve you and your enemy's races. enemy() will just return the first enemy.
        // If you wish to deal with multiple enemies then you must use enemies().
        if (BWAPI::Broodwar->enemy()) { // First make sure there is an enemy
            BWAPI::Broodwar << "Matchup is " << BWAPI::Broodwar->self()->getRace() << " vs " << BWAPI::Broodwar->enemy()->getRace() << std::endl;
        }
    }

}

void AresAIModule::onEnd(bool isWinner)
{
    NOTIFY_LISTENERS(onEnd, isWinner)

    // Called when the game ends
    if (isWinner)
    {
        BWAPI::Broodwar << "AresBWAPIBot won the game." << std::endl;
    }
}

void AresAIModule::onSaveGame(std::string gameName)
{
    BWAPI::Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void AresAIModule::onFrame()
{
    NOTIFY_LISTENERS(onFrame)

    // Display the game frame rate as text in the upper left area of the screen
    BWAPI::Broodwar->drawTextScreen(200, 0,  "FPS: %d",         BWAPI::Broodwar->getFPS() );
    BWAPI::Broodwar->drawTextScreen(200, 20, "Average FPS: %f", BWAPI::Broodwar->getAverageFPS() );

    // Return if the game is a replay or is paused
    if (BWAPI::Broodwar->isReplay() || BWAPI::Broodwar->isPaused() || !BWAPI::Broodwar->self()) {
        return;
    }

    // Prevent spamming by only running our onFrame once every number of latency frames.
    // Latency frames are the number of frames before commands are processed.
    if (BWAPI::Broodwar->getFrameCount() % BWAPI::Broodwar->getLatencyFrames() != 0) {
        return;
    }

    // Iterate through all the units that we own
    for (auto& u : BWAPI::Broodwar->self()->getUnits())
    {
        // Ignore the unit if it no longer exists
        // Make sure to include this block when handling any Unit pointer!
        if (!u->exists()) {
            continue;
        }

        // Ignore the unit if it has one of the following status ailments
        if (u->isLockedDown() || u->isMaelstrommed() || u->isStasised()) {
            continue;
        }

        // Ignore the unit if it is in one of the following states
        if (u->isLoaded() || !u->isPowered() || u->isStuck()) {
            continue;
        }

        // Ignore the unit if it is incomplete or busy constructing
        if (!u->isCompleted() || u->isConstructing()) {
            continue;
        }

        // If the unit is a worker unit
        if (u->getType().isWorker())
        {
            // if our worker is idle
            if (u->isIdle())
            {
                // Order workers carrying a resource to return them to the center,
                // otherwise find a mineral patch to harvest.
                if (u->isCarryingGas() || u->isCarryingMinerals())
                {
                    u->returnCargo();
                }
                else if (!u->getPowerUp())  // The worker cannot harvest anything if it
                {                           // is carrying a powerup such as a flag
                    // Harvest from the nearest mineral patch or gas refinery
                    if (!u->gather(u->getClosestUnit(BWAPI::Filter::IsMineralField || BWAPI::Filter::IsRefinery)))
                    {
                        // If the call fails, then print the last error message
                        BWAPI::Broodwar << BWAPI::Broodwar->getLastError() << std::endl;
                    }

                } // closure: has no powerup
            } // closure: if idle
        }
        else if (u->getType().isResourceDepot()) // A resource depot is a Command Center, Nexus, or Hatchery
        {
            // Order the depot to construct more workers! But only when it is idle.
            if (u->isIdle() && !u->train(u->getType().getRace().getWorker()))
            {
                // If that fails, draw the error at the location so that you can visibly see what went wrong!
                // However, drawing the error once will only appear for a single frame
                // so create an event that keeps it on the screen for some frames
                BWAPI::Position pos = u->getPosition();
                BWAPI::Error lastErr = BWAPI::Broodwar->getLastError();
                BWAPI::Broodwar->registerEvent([pos,lastErr](BWAPI::Game*){
                        BWAPI::Broodwar->drawTextMap(pos, "%c%s", BWAPI::Text::White, lastErr.c_str());
                    }, // action
                    nullptr, // condition
                    BWAPI::Broodwar->getLatencyFrames());  // frames to run

                // Retrieve the supply provider type in the case that we have run out of supplies
                BWAPI::UnitType supplyProviderType = u->getType().getRace().getSupplyProvider();
                static int lastChecked = 0;

                // If we are supply blocked and haven't tried constructing more recently
                if (lastErr == BWAPI::Errors::Insufficient_Supply &&
                    lastChecked + 400 < BWAPI::Broodwar->getFrameCount() &&
                    BWAPI::Broodwar->self()->incompleteUnitCount(supplyProviderType) == 0)
                {
                    lastChecked = BWAPI::Broodwar->getFrameCount();

                    // Retrieve a unit that is capable of constructing the supply needed
                    BWAPI::Unit supplyBuilder = u->getClosestUnit(BWAPI::Filter::GetType == supplyProviderType.whatBuilds().first
                                                                  && (BWAPI::Filter::IsIdle || BWAPI::Filter::IsGatheringMinerals)
                                                                  && BWAPI::Filter::IsOwned);
                    // If a unit was found
                    if (supplyBuilder)
                    {
                        if (supplyProviderType.isBuilding())
                        {
                            BWAPI::TilePosition targetBuildLocation = BWAPI::Broodwar->getBuildLocation(supplyProviderType, supplyBuilder->getTilePosition());
                            if (targetBuildLocation)
                            {
                                // Register an event that draws the target build location
                                BWAPI::Broodwar->registerEvent([targetBuildLocation, supplyProviderType](BWAPI::Game*) {
                                        BWAPI::Broodwar->drawBoxMap( BWAPI::Position(targetBuildLocation),
                                            BWAPI::Position(targetBuildLocation + supplyProviderType.tileSize()),
                                            BWAPI::Colors::Blue);
                                    },
                                    nullptr,  // condition
                                    supplyProviderType.buildTime() + 100 );  // frames to run

                                // Order the builder to construct the supply structure
                                supplyBuilder->build(supplyProviderType, targetBuildLocation);
                            }
                        }
                        else
                        {
                            // Train the supply provider (Overlord) if the provider is not a structure
                            supplyBuilder->train(supplyProviderType);
                        }
                    } // closure: supplyBuilder is valid
                } // closure: insufficient supply
            } // closure: failed to train idle unit
        }
    } // closure: unit iterator
}

void AresAIModule::onSendText(std::string text)
{
    // Send the text to the game if it is not being processed.
    BWAPI::Broodwar->sendText("%s", text.c_str());
    // Make sure to use %s and pass the text as a parameter,
    // otherwise you may run into problems when you use the %(percent) character!
}

void AresAIModule::onReceiveText(BWAPI::Player player, std::string text)
{
    // Parse the received text
    BWAPI::Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void AresAIModule::onPlayerLeft(BWAPI::Player player)
{
    // Interact verbally with the other players in the game by
    // announcing that the other player has left.
    BWAPI::Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void AresAIModule::onNukeDetect(BWAPI::Position target)
{
    // Check if the target is a valid BWAPI::Position
    if (target)
    {
        // if so, print the location of the nuclear strike target
        BWAPI::Broodwar << "Nuclear Launch Detected at " << target << std::endl;
    }
    else
    {
        // Otherwise, ask other players where the nuke is!
        BWAPI::Broodwar->sendText("Where's the nuke?");
    }

    // You can also retrieve all the nuclear missile targets using BWAPI::Broodwar->getNukeDots()!
}

void AresAIModule::onUnitCreate(BWAPI::Unit unit)
{
    if (BWAPI::Broodwar->isReplay())
    {
        // If we are in a replay, then we will print out the build order of the structures
        if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
        {
            int seconds = BWAPI::Broodwar->getFrameCount()/24;
            int minutes = seconds/60;
            seconds %= 60;
            BWAPI::Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
        }
    }
}

void AresAIModule::onUnitMorph(BWAPI::Unit unit)
{
    if (BWAPI::Broodwar->isReplay())
    {
        // if we are in a replay, then we will print out the build order of the structures
        if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
        {
            int seconds = BWAPI::Broodwar->getFrameCount()/24;
            int minutes = seconds/60;
            seconds %= 60;
            BWAPI::Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
        }
    }
}

void AresAIModule::suscribe(abc::GameEventListener& listener)
{
    m_gameListeners.emplace_back(listener);
}

void AresAIModule::unsuscribe(abc::GameEventListener& listener)
{
    m_gameListeners.remove_if([&listener](m_gameListeners::value_type listenRef) -> bool {
        return &listenRef.get() == &listener;
    });
}

void suscribe(abc::WorkerEventListener& listener)
{
    m_workerListeners.emplace_back(listener);
}

void unsuscribe(abc::WorkerEventListener& listener)
{
    m_eventListeners.remove_if([&listener](m_eventListeners::value_type listenRef) -> bool {
        return &listenRef.get() == &listener;
    });
}

}

