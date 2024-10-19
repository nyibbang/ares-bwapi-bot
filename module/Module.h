#pragma once

#include <BWAPI/AIModule.h>
#include <quill/Logger.h>

namespace ares::module {

class Module final : public BWAPI::AIModule {
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
  quill::Logger *_logger;
};

} // namespace ares::module
