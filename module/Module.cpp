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

#include "Module.h"
#include "core/ResourcesHarvester.h"
#include "core/abc/GameEventListener.h"
#include "core/abc/WorkerEventListener.h"
#include "core/trace/Trace.h"
#include "core/trace/Logger.h"
#include <iostream>

#define NOTIFY_LISTENERS(type, container, func, ...) \
    for (type* listener : container) { \
        listener->func(__VA_ARGS__); \
    }
#define NOTIFY_GAME_LISTENERS(func, ...) \
    NOTIFY_LISTENERS(core::abc::GameEventListener, m_gameListeners, func, __VA_ARGS__)
#define NOTIFY_WORKER_LISTENERS(func, ...) \
    NOTIFY_LISTENERS(core::abc::WorkerEventListener, m_workerListeners, func, __VA_ARGS__)

namespace
{
class BroodwarLogger final : public trace::abc::Logger
{
    public:
        void log(const trace::LogContext&, const std::string& message) override
        {
            // Ignore the context, just print the message
            BWAPI::Broodwar << message << std::endl;
        }
};
}

namespace ares
{
namespace module
{

void Commander::execute(ares::core::CommandType type, int unitId)
{
    using ares::core::CommandType;
    BWAPI::Unit unit = BWAPI::Broodwar->getUnit(unitId);
    switch (type)
    {
        case CommandType::HarvestClosestMineral:
            unit->gather(unit->getClosestUnit(BWAPI::Filter::IsMineralField));
            break;
    }
}

Module::Module()
{
    trace::Facade::initializeAuxiliaryLogger(trace::LoggerPtr(new BroodwarLogger));
}

void Module::onStart()
{
    // Initialize members
    m_commander.reset(new Commander);
    m_rscHvst.reset(new core::ResourcesHarvester(*this, *m_commander));

    // We do not care about replays
    if (BWAPI::Broodwar->isReplay()) {
        return;
    }

    /* Set the command optimization level so that common commands can
       be grouped and reduce the bot's APM (Actions Per Minute). */
    BWAPI::Broodwar->setCommandOptimizationLevel(2);

    NOTIFY_GAME_LISTENERS(onStart)

#ifdef ARES_DEBUG_BUILD
    BWAPI::Broodwar->sendText("Ares version %d.%d.%d-debug", ARES_MAJOR_VERSION,
                              ARES_MINOR_VERSION, ARES_PATCH_VERSION);
    BWAPI::Broodwar->registerEvent([](BWAPI::Game*){
        BWAPI::Broodwar->drawTextScreen(BWAPI::Positions::Origin, "%cAres v%d.%d.%d",
                                        BWAPI::Text::Red,
                                        ARES_MAJOR_VERSION, ARES_MINOR_VERSION, ARES_PATCH_VERSION);
        });
#endif

    ARES_DEBUG() << "AresBWAPIBot version " << ARES_MAJOR_VERSION << "." << ARES_MINOR_VERSION
                 << "." << ARES_PATCH_VERSION;
    ARES_DEBUG() << "New game started on map \"" << BWAPI::Broodwar->mapName() << "\"";
    ARES_DEBUG() << "Matchup is Ares (" << BWAPI::Broodwar->self()->getRace()
                 << ") vs Enemy (" << BWAPI::Broodwar->enemy()->getRace() << ")";
}

void Module::onEnd(bool isWinner)
{
    NOTIFY_GAME_LISTENERS(onEnd, isWinner)
    ARES_DEBUG() << "Game is finished, Ares " << (isWinner ? "won" : "lost") << " the game";
}

void Module::onSaveGame(std::string gameName)
{
    ARES_DEBUG() << "Game was saved as " << gameName;
}

void Module::onFrame()
{
    /* Return if the game is a replay or is paused
       Also prevent spamming by only running our onFrame once every number of latency frames.
       Latency frames are the number of frames before commands are processed. */
    if (BWAPI::Broodwar->isReplay() || BWAPI::Broodwar->isPaused() || !BWAPI::Broodwar->self()
           || BWAPI::Broodwar->getFrameCount() % BWAPI::Broodwar->getLatencyFrames() != 0)
    {
        return;
    }

    // Iterate through all the units that we own
    for (auto&& u : BWAPI::Broodwar->self()->getUnits())
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

        // If the unit is a worker unit and it is idle
        if (u->getType().isWorker() && u->isIdle())
        {
            const auto unitID = u->getID();
            NOTIFY_WORKER_LISTENERS(onWorkerIdle, unitID);
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
                    }
                }
            }
        }
    }

    NOTIFY_GAME_LISTENERS(onFrame)
}

void Module::onSendText(std::string text)
{
    BWAPI::Broodwar->sendText("%s", text.c_str());
}

void Module::onReceiveText(BWAPI::Player player, std::string text)
{
    const std::string playerName = (player == BWAPI::Broodwar->self() ?
                                        "Ares" : player->getName());
    const std::string message = playerName + " said \"" + text + "\"";
    ARES_INFO() << message;
    BWAPI::Broodwar << message << std::endl;
}

void Module::onPlayerLeft(BWAPI::Player player)
{
    const std::string message = player->getName() + " has left the game";
    ARES_INFO() << message;
    BWAPI::Broodwar << message << std::endl;
}

void Module::onNukeDetect(BWAPI::Position target)
{
    std::ostringstream oss;
    oss << "Nuclear launch detected";
    if (target) {
        oss << " at " << target;
    }
    ARES_DEBUG() << oss.str();
}

void Module::onUnitCreate(BWAPI::Unit unit)
{
    // We do not care about replays
    if (BWAPI::Broodwar->isReplay()) {
        return;
    }
    BWAPI::Player player = unit->getPlayer();
    const std::string playerName = (player == BWAPI::Broodwar->self() ?
                                        "Ares" : player->getName());
    ARES_DEBUG() << playerName << " created unit of type " << unit->getType();
}

void Module::onUnitMorph(BWAPI::Unit unit)
{
    // We do not care about replays
    if (BWAPI::Broodwar->isReplay()) {
        return;
    }
    BWAPI::Player player = unit->getPlayer();
    const std::string playerName = (player == BWAPI::Broodwar->self() ?
                                        "Ares" : player->getName());
    ARES_DEBUG() << playerName << " morphed unit of type " << unit->getType();
}

void Module::suscribe(core::abc::GameEventListener& listener)
{
    m_gameListeners.push_front(&listener);
}

void Module::unsuscribe(core::abc::GameEventListener& listener)
{
    m_gameListeners.remove(&listener);
}

void Module::suscribe(core::abc::WorkerEventListener& listener)
{
    m_workerListeners.push_front(&listener);
}

void Module::unsuscribe(core::abc::WorkerEventListener& listener)
{
    m_workerListeners.remove(&listener);
}

}
}
