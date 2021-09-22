#include "tab_about.h"

#include <imgui.h>
#include "../git_info.h"

void gsf::menu::tab_about::render_tab()
{
    if (ImGui::BeginTabItem("About"))
    {
        ImGui::Text(
            "Scripting API and Framework for Genshin Impact\n"
            "\n"

            "Build type: "
            #if defined(_DEBUG) && !defined(NDEBUG)
            "Debug"
            #elif defined(NDEBUG) && !defined(_DEBUG)
            "Release"
            #else
            "Unknown"
            #endif
            "\n"

            "Build Date: " __DATE__ " " __TIME__ "\n"
            "Git info: " GIT_HASH " @ " GIT_BRANCH "\n"
            "\n"
            "https://github.com/u16rogue/genshin-scripting-framework\n"
            "\n"
            "This software is licensed under the GNU General Public License 3.0\n"
        );
        ImGui::EndTabItem();
    }
}