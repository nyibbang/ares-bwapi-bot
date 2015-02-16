# Ares BWAPI Bot
Ares is aimed to become a capable bot for Starcraft Brood War using BWAPI developped in C++.
It is named after the greek god of war.

## Instructions
### Requirements
  * Install **StarCraft:BroodWar** and update it to version **1.16.1** by going in Multiplayer/Battle.net(Europe)
  * Install the latest release of **BWAPI** from https://github.com/bwapi/bwapi/releases

### Building AresBWAPIBot under VisualStudio
  * Install **CMake** (tested under CMake 3.2.0-rc1)
  * Run **cmake-gui**
  * Put Ares source path in the field "Where is the source code"
  * Put your build path in the field "Where to build the binaries" (eg *Ares/build*)
  * Configure (use the appropriate generator when asked, depending on your version of VisualStudio)
  * Generate
  * Open the generated solution **AresBWAPIBot.sln** in your build folder
  * Build the solution (in either Debug or Release mode)

### Running AresBWAPIBot in SC:BW
  * Copy **AresModule.dll** (or **AresModuled.dll**) from *\<Ares_build_folder\>/module/Debug* (or *Release*) to *\<Starcraft_install_folder\>/bwapi-data/AI*
  * Modify BWAPI configuration file in *\<Starcraft_install_folder\>/bwapi-data/bwapi.ini*
    * Change *ai* et *ai_debug* entries:
```
      ai     = bwapi-data/AI/AresModule.dll
      ai_dbg = bwapi-data/AI/AresModuled.dll
```
  * Run **ChaosLauncher** from *\<BWAPI_install_folder\>/ChaosLauncher*
    * Activate plugin **BWAPI Injector 1.16.1 DEBUG** (or **RELEASE** if you want to use the release version of the dll)
    * Press Run
