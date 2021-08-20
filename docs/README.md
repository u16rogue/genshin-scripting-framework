# Genshin Impact Scripting Framework

[![release](https://img.shields.io/github/release/u16rogue/genshin-scripting-framework?style=for-the-badge)](https://github.com/u16rogue/genshin-scripting-framework/releases)
[![issues](https://img.shields.io/github/issues/u16rogue/genshin-scripting-framework?style=for-the-badge)](https://github.com/u16rogue/genshin-scripting-framework/issues)
[![license](https://img.shields.io/github/license/u16rogue/genshin-scripting-framework?style=for-the-badge)](https://github.com/u16rogue/genshin-scripting-framework/blob/master/LICENSE)


[About](#About) • [Getting Started](#Getting-Started) • [Usage](#Usage) • [Scripting API Documentation](script_api.md) • [Change Logs](change_logs.md) • [Libraries](#Libraries) • [License](#License) • [Todo](#Todo)

![](client_ss.jpg)

## Repository
| Branch                                                                              | Status                                                                                                                                                                                                                                                                                 | Commit Count                                                                                                                  | Last Commit                                                                                           
|:-----------------------------------------------------------------------------------:|:--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|:-----------------------------------------------------------------------------------------------------------------------------:|:--------------------------------------------------------------------------------------------------------------------- |
| [master](https://github.com/u16rogue/genshin-scripting-framework/tree/master) | [![Build Status](https://img.shields.io/endpoint.svg?url=https%3A%2F%2Factions-badge.atrox.dev%2Fu16rogue%2Fgenshin-scripting-framework%2Fbadge%3Fref%3Dmaster&style=for-the-badge)](https://actions-badge.atrox.dev/u16rogue/genshin-scripting-framework/goto?ref=master) | ![](https://img.shields.io/github/commits-since/u16rogue/genshin-scripting-framework/latest/master?style=for-the-badge) | ![](https://img.shields.io/github/last-commit/u16rogue/genshin-scripting-framework/master?style=for-the-badge)  |

<!--- | [dev](https://github.com/u16rogue/genshin-scripting-framework/tree/dev)       | [![Build Status](https://img.shields.io/endpoint.svg?url=https%3A%2F%2Factions-badge.atrox.dev%2Fu16rogue%2Fgenshin-scripting-framework%2Fbadge%3Fref%3Ddev&style=for-the-badge)](https://actions-badge.atrox.dev/u16rogue/genshin-scripting-framework/goto?ref=dev)       | ![](https://img.shields.io/github/commits-since/u16rogue/genshin-scripting-framework/latest/dev?style=for-the-badge)    | ![](https://img.shields.io/github/last-commit/u16rogue/genshin-scripting-framework/dev?style=for-the-badge)     | --->

## About
Provides a scripting framework and API for the game [Genshin Impact](https://genshin.mihoyo.com/).

#### Disclaimer:
* *GSF is not a cheat/hack. GSF does not contain any "advantage" enchancement features built in. GSF only provides a framework and API to its users.*
* *The contributors of this repository/project/software bear no responsibility on whatever purpose the user does with this.*

## Getting Started

### Prerequisite
* Windows
* [DirectX 11 SDK](https://www.microsoft.com/en-au/download/details.aspx?id=6812)
* [Visual Studio 2019](https://visualstudio.microsoft.com/) or greater
* [Git](https://git-scm.com/downloads)

### Cloning
This requires [Git](https://git-scm.com/). If you don't have Git yet you can [download it here](https://git-scm.com/downloads).

* If you are planning on contributing or want access to the repo's history locally:
    
    ```sh
    git clone https://github.com/u16rogue/genshin-scripting-framework.git
    ```

* If you only want to compile and use GSF:

    ```sh
    git clone --depth 1 https://github.com/u16rogue/genshin-scripting-framework.git
    ```

### Projects

* `gsf-client` - The client that's loaded into the game, this provides the scripting API and ability to load LUA scripts.
* `gsf-launcher` - (*optional*) Used for launching the game and loading `gsf-client` into it. Requires gsf-client to be built (This is already set up in the project as a dependency).

### Compiling
The project is ready to compile immediately, you can proceed by opening the solution file, set up the configuration (Release x64), and start building.

*If you get an error regarding a missing macro (**GIT_HASH** and **GIT_BRANCH**) and a header file (**git_info.h**), simply build the project once and it should fix the problem. The macro and header file is automatically generated pre-build.*

### Output
The built binaries are located at `build\Release_x64` for release builds and `build\Debug_x64` for debug builds.

### Implementing an autoexec lua script
An autoexec lua script is a script that automatically gets loaded and executed the moment GSF is loaded. To implement:
1. Create a header file named `autoexecdef.h` in `gsf-client`. (`gsf-client\autoexecdef.h`)
1. Create a define named `GSF_AUTOEXEC_SCRIPT_PATH` and set the value as the **absolute** path of your designated autoexec script (`#define GSF_AUTOEXEC_SCRIPT_PATH "C:\autoexec.lua"`)

**Note:** `autoexec` scripts is limited to 2 API's which is [gsf.import](script_api.md#gsfimportfile_path) and [gsf.load](script_api.md#gsfloadfile_path)

## Usage

### Prerequisite
* [Microsoft Visual C++ Redistributable for Visual Studio 2019](https://visualstudio.microsoft.com/downloads/#microsoft-visual-c-redistributable-for-visual-studio-2019)
* *(optional)* Bypass for mhyprot2

### Injecting
General usage of GSF is by injecting `gsf-client.dll` to `GenshinImpact.exe`. Upon successful injection a top menu bar should appear ingame where you can access GSF's features.

You can also use `gsf-launcher` to load GSF to Genshin Impact. if you are using this method make sure that the game is currently not running and to execute the launcher with administrator privilege.

**Note:** `gsf-launcher` does **NOT** disable the anti-cheat mhyprot2. GSF might load in-game but the anti-cheat is still active in the background, do also take note that both the launcher and client do not provide *evasion* against detection (if any is in place anyway).

### Loading Lua Scripts
Lua Scripts can be loaded through the `Script Manager` window which can be accessed by opening the `Genshin Scripting Framework` dropdown on the top menu bar. Inside the `Script Manager` window you can import Lua scripts through the `Import` dropdown, upon a successful import the script should appear in the window's list where a button is provided for loading the script.

### Unloading / Shutting down GSF
You can either use the <kbd>DELETE</kbd> key or the `Shutdown` option under GSF's dropdown on the top menu bar.

## Libraries
* [MinHook](https://github.com/TsudaKageyu/minhook) 1.3.3 - TsudaKageyu
* [Dear ImGui](https://github.com/ocornut/imgui) 1.80 - ocornut
* [Lua](https://www.lua.org/) 5.4.3 - Lua org, PUC-Rio
* [sol2](https://github.com/ThePhD/sol2) 3.2.2 - Rapptz, ThePhD, and contributors

## License
[GNU General Public License 3.0](https://www.gnu.org/licenses/gpl-3.0.en.html)

## Todo
* xlua interfacing from GSF
* imported script list persistence