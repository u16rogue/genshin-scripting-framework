# Genshin Impact Scripting Framework

[![release](https://img.shields.io/github/release/genshcript-dev/genshin-scripting-framework?style=for-the-badge)](https://github.com/genshcript-dev/genshin-scripting-framework/releases)
[![issues](https://img.shields.io/github/issues/genshcript-dev/genshin-scripting-framework?style=for-the-badge)](https://github.com/genshcript-dev/genshin-scripting-framework/issues)
[![license](https://img.shields.io/github/license/genshcript-dev/genshin-scripting-framework?style=for-the-badge)](https://github.com/genshcript-dev/genshin-scripting-framework/blob/master/LICENSE)


[About](#About) • [Getting Started](#Getting-Started) • [Scripting API Documentation](script_api.md) • [Change Logs](change_logs.md) • [Libraries](#Libraries) • [License](#License)

![](client_ss.jpg)

## Repository
| Branch                                                                              | Status                                                                                                                                                                                                                                                                                 | Commit Count                                                                                                                  | Last Commit                                                                                           
|:-----------------------------------------------------------------------------------:|:--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|:-----------------------------------------------------------------------------------------------------------------------------:|:--------------------------------------------------------------------------------------------------------------------- |
| [master](https://github.com/genshcript-dev/genshin-scripting-framework/tree/master) | [![Build Status](https://img.shields.io/endpoint.svg?url=https%3A%2F%2Factions-badge.atrox.dev%2Fgenshcript-dev%2Fgenshin-scripting-framework%2Fbadge%3Fref%3Dmaster&style=for-the-badge)](https://actions-badge.atrox.dev/genshcript-dev/genshin-scripting-framework/goto?ref=master) | ![](https://img.shields.io/github/commits-since/genshcript-dev/genshin-scripting-framework/latest/master?style=for-the-badge) | ![](https://img.shields.io/github/last-commit/genshcript-dev/genshin-scripting-framework/master?style=for-the-badge)  |

<!--- | [dev](https://github.com/genshcript-dev/genshin-scripting-framework/tree/dev)       | [![Build Status](https://img.shields.io/endpoint.svg?url=https%3A%2F%2Factions-badge.atrox.dev%2Fgenshcript-dev%2Fgenshin-scripting-framework%2Fbadge%3Fref%3Ddev&style=for-the-badge)](https://actions-badge.atrox.dev/genshcript-dev/genshin-scripting-framework/goto?ref=dev)       | ![](https://img.shields.io/github/commits-since/genshcript-dev/genshin-scripting-framework/latest/dev?style=for-the-badge)    | ![](https://img.shields.io/github/last-commit/genshcript-dev/genshin-scripting-framework/dev?style=for-the-badge)     | --->

## About
Provides a scripting framework and API for the game [Genshin Impact](https://genshin.mihoyo.com/).

*GSF is not a cheat/hack. GSF does not contain any "advantage" enchancement features built in. GSF only provides a framework and API to its users.*

## Getting Started

### Prerequisite
* Windows
* [DirectX SDK](https://www.microsoft.com/en-au/download/details.aspx?id=6812)
* [Visual Studio 2019](https://visualstudio.microsoft.com/)
* [Git](https://git-scm.com/downloads)

### Cloning
This requires [Git](https://git-scm.com/). If you don't have Git yet you can [download it here](https://git-scm.com/downloads).

* If you are planning on contributing or want access to the repo's history locally:
    
    ```sh
    git clone https://github.com/rogueeeee/genshin-scripting-framework.git
    ```

* If you only want to compile and use GSF:

    ```sh
    git clone --depth 1 https://github.com/rogueeeee/genshin-scripting-framework.git
    ```

### Compiling
The project is ready to compile immediately, you can proceed by opening the solution file, set up the configuration (Release x64), and start building.

*If you get an error regarding a missing macro (**GSF_CLIENT_GIT_HASH**) and a header file (**git_hash.h**), simply build the project once and it should fix the problem. The macro and header file is automatically generated pre-build.*

### Output
The built binaries are located at `build\Release_x64` for release builds and `build\Debug_x64` debug builds.

## Libraries
* [MinHook](https://github.com/TsudaKageyu/minhook) 1.3.3 - TsudaKageyu
* [Dear ImGui](https://github.com/ocornut/imgui) 1.80 - ocornut
* [Lua](https://www.lua.org/) 5.4.3 - Lua org, PUC-Rio
* [sol2](https://github.com/ThePhD/sol2) 3.2.2 - Rapptz, ThePhD, and contributors

## License
[GNU General Public License 3.0](https://www.gnu.org/licenses/gpl-3.0.en.html)