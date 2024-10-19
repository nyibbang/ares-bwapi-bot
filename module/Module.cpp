#include "Module.h"
#include "config.h"
#include <BWAPI/Game.h>
#include <BWAPI/Player.h>
#include <BWAPI/Position.h>
#include <BWAPI/Unitset.h>
#include <quill/Backend.h>
#include <quill/Frontend.h>
#include <quill/LogMacros.h>
#include <quill/Logger.h>
#include <quill/bundled/fmt/format.h>
#include <quill/bundled/fmt/ostream.h>
#include <quill/sinks/FileSink.h>

template <typename T, int Scale>
struct fmtquill::formatter<BWAPI::Point<T, Scale>>
    : fmtquill::ostream_formatter {};

namespace ares::module {

namespace {

std::string name(BWAPI::Player player) {
  return (player == BWAPI::Broodwar->self() ? "Ares" : player->getName());
}

} // namespace

Module::Module() {
  quill::Backend::start();
  auto file_sink = quill::Frontend::create_or_get_sink<quill::FileSink>(
      "ares_bwapi_bot.log", []() {
        quill::FileSinkConfig cfg;
        cfg.set_open_mode('w');
        cfg.set_filename_append_option(
            quill::FilenameAppendOption::StartDateTime);
        return cfg;
      }());
  _logger =
      quill::Frontend::create_or_get_logger("module", std::move(file_sink));
  _logger->set_log_level(
#ifdef _DEBUG
      quill::LogLevel::Debug
#else
      quill::LogLevel::Info
#endif
  );
}

void Module::onStart() {
  // We do not care about replays
  if (BWAPI::Broodwar->isReplay()) {
    return;
  }

  /* Set the command optimization level so that common commands can
     be grouped and reduce the bot's APM (Actions Per Minute). */
  BWAPI::Broodwar->setCommandOptimizationLevel(2);

#ifdef _DEBUG
  BWAPI::Broodwar->sendText("Ares version %d.%d.%d-debug", ARES_MAJOR_VERSION,
                            ARES_MINOR_VERSION, ARES_PATCH_VERSION);
  BWAPI::Broodwar->registerEvent([](BWAPI::Game *) {
    BWAPI::Broodwar->drawTextScreen(
        BWAPI::Positions::Origin, "%cAres v%d.%d.%d", BWAPI::Text::Red,
        ARES_MAJOR_VERSION, ARES_MINOR_VERSION, ARES_PATCH_VERSION);
  });
#endif

  LOG_DEBUG(_logger, "AresBWAPIBot version {}.{}.{}", ARES_MAJOR_VERSION,
            ARES_MINOR_VERSION, ARES_PATCH_VERSION);
  LOG_DEBUG(_logger, "New game started on map \"{}\"",
            BWAPI::Broodwar->mapName());
  LOG_DEBUG(_logger, "Matchup is Ares \"{}\" vs Enemy \"{}\"",
            BWAPI::Broodwar->self()->getRace().getName(),
            BWAPI::Broodwar->enemy()->getRace().getName());
}

void Module::onEnd(bool isWinner) {
  LOG_DEBUG(_logger, "Game is finished, Ares {} the game",
            (isWinner ? "won" : "lost"));
}

void Module::onSaveGame(std::string gameName) {
  LOG_DEBUG(_logger, "Game was saved as {}", gameName);
}

namespace {
BWAPI::Unitset filterSelfUnits(BWAPI::UnitFilter filter) {
  BWAPI::Unitset filteredUnitSet;
  auto units = BWAPI::Broodwar->self()->getUnits();
  for (auto &&unit : units) {
    if (unit && filter(unit)) {
      filteredUnitSet.insert(unit);
    }
  }
  return filteredUnitSet;
}

bool unitIsActive(BWAPI::Unit unit) {
  return (BWAPI::Filter::Exists && !BWAPI::Filter::IsLockedDown &&
          !BWAPI::Filter::IsMaelstrommed && !BWAPI::Filter::IsStasised &&
          !BWAPI::Filter::IsLoaded && BWAPI::Filter::IsPowered &&
          !BWAPI::Filter::IsStuck && BWAPI::Filter::IsCompleted &&
          !BWAPI::Filter::IsConstructing)(unit);
}

bool unitIsIdleWorker(BWAPI::Unit unit) {
  return (BWAPI::PtrUnitFilter(&unitIsActive) && BWAPI::Filter::IsWorker &&
          BWAPI::Filter::IsIdle)(unit);
}

bool unitIsResourceDepot(BWAPI::Unit unit) {
  return (BWAPI::PtrUnitFilter(&unitIsActive) &&
          BWAPI::Filter::IsResourceDepot)(unit);
}

} // namespace

void Module::onFrame() {
  // Return if the game is a replay or is paused
  // Also prevent spamming by only running our onFrame once every number of
  // latency frames. Latency frames are the number of frames before commands are
  // processed.
  if (BWAPI::Broodwar->isReplay() || BWAPI::Broodwar->isPaused() ||
      !BWAPI::Broodwar->self() ||
      BWAPI::Broodwar->getFrameCount() % BWAPI::Broodwar->getLatencyFrames() !=
          0) {
    return;
  }

  sendIdleWorkersToMinerals();
  buildWorkers();
}

void Module::onSendText(std::string text) {
  BWAPI::Broodwar->sendText("%s", text.c_str());
}

void Module::onReceiveText(BWAPI::Player player, std::string text) {
  LOG_INFO(_logger, "{} said \"{}\"", name(player), text);
}

void Module::onPlayerLeft(BWAPI::Player player) {
  LOG_INFO(_logger, "{} has left the game", player->getName());
}

void Module::onNukeDetect(BWAPI::Position) {}

void Module::onUnitCreate(BWAPI::Unit unit) {
  // We do not care about replays
  if (BWAPI::Broodwar->isReplay()) {
    return;
  }
}

void Module::onUnitMorph(BWAPI::Unit unit) {
  // We do not care about replays
  if (BWAPI::Broodwar->isReplay()) {
    return;
  }
}

void Module::sendIdleWorkersToMinerals() {
  // send idle workers to mineral fields
  for (auto &&unit : BWAPI::Broodwar->self()->getUnits()) {
    if (unitIsIdleWorker(unit)) {
      unit->gather(unit->getClosestUnit(BWAPI::Filter::IsMineralField));
    }
  }
}

void Module::buildWorkers() {
  for (auto &&resourceDepot : filterSelfUnits(&unitIsResourceDepot)) {
    // Order the depot to construct more workers! But only when it is idle.
    if (resourceDepot->isIdle() &&
        !resourceDepot->train(resourceDepot->getType().getRace().getWorker())) {
      // If that fails, draw the error at the location so that you can visibly
      // see what went wrong! However, drawing the error once will only appear
      // for a single frame so create an event that keeps it on the screen for
      // some frames
      BWAPI::Position pos = resourceDepot->getPosition();
      BWAPI::Error lastErr = BWAPI::Broodwar->getLastError();
      BWAPI::Broodwar->registerEvent(
          [pos, lastErr](BWAPI::Game *) {
            BWAPI::Broodwar->drawTextMap(pos, "%c%s", BWAPI::Text::White,
                                         lastErr.c_str());
          },                                    // action
          nullptr,                              // condition
          BWAPI::Broodwar->getLatencyFrames()); // frames to run

      // Retrieve the supply provider type in the case that we have run out of
      // supplies
      BWAPI::UnitType supplyProviderType =
          resourceDepot->getType().getRace().getSupplyProvider();
      static int lastChecked = 0;

      // If we are supply blocked and haven't tried constructing more recently
      if (lastErr == BWAPI::Errors::Insufficient_Supply &&
          lastChecked + 400 < BWAPI::Broodwar->getFrameCount() &&
          BWAPI::Broodwar->self()->incompleteUnitCount(supplyProviderType) ==
              0) {
        lastChecked = BWAPI::Broodwar->getFrameCount();

        // Retrieve a unit that is capable of constructing the supply needed
        BWAPI::Unit supplyBuilder = resourceDepot->getClosestUnit(
            BWAPI::Filter::GetType == supplyProviderType.whatBuilds().first &&
            (BWAPI::Filter::IsIdle || BWAPI::Filter::IsGatheringMinerals) &&
            BWAPI::Filter::IsOwned);
        // If a unit was found
        if (supplyBuilder) {
          if (supplyProviderType.isBuilding()) {
            BWAPI::TilePosition targetBuildLocation =
                BWAPI::Broodwar->getBuildLocation(
                    supplyProviderType, supplyBuilder->getTilePosition());
            if (targetBuildLocation) {
              // Register an event that draws the target build location
              BWAPI::Broodwar->registerEvent(
                  [targetBuildLocation, supplyProviderType](BWAPI::Game *) {
                    BWAPI::Broodwar->drawBoxMap(
                        BWAPI::Position(targetBuildLocation),
                        BWAPI::Position(targetBuildLocation +
                                        supplyProviderType.tileSize()),
                        BWAPI::Colors::Blue);
                  },
                  nullptr,                               // condition
                  supplyProviderType.buildTime() + 100); // frames to run

              // Order the builder to construct the supply structure
              supplyBuilder->build(supplyProviderType, targetBuildLocation);
            }
          } else {
            // Train the supply provider (Overlord) if the provider is not a
            // structure
            supplyBuilder->train(supplyProviderType);
          }
        }
      }
    }
  }
}

} // namespace ares::module
