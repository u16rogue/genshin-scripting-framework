#include <Windows.h>
#include <filesystem>
#include <loadlibrary.h>
#include <winapi_helper.h>
#include <macro.h>

#define GAME_DIRECTORY \
    L"C:\\Program Files\\Genshin Impact\\Genshin Impact game"

#define GAME_PATH \
    GAME_DIRECTORY L"\\GenshinImpact.exe"

/* #define MSG_TITLE */ const wchar_t * const MSG_TITLE = L"gsf-launcher";

void msgprompt(const wchar_t *msg)
{
    MessageBoxW(nullptr, msg, MSG_TITLE, MB_OK);
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
    #if _DEBUG
    con::init();
    #endif

    HANDLE game_handle = nullptr;
    HANDLE game_thread = nullptr;

    if (std::filesystem::exists(GAME_PATH))
    {
        STARTUPINFO         si { sizeof(si) };
        PROCESS_INFORMATION pi {    NULL    };

        if (CreateProcessW(GAME_PATH, nullptr, nullptr, nullptr, FALSE, NULL, nullptr, GAME_DIRECTORY, &si, &pi))
        {
            game_handle = pi.hProcess;
            game_thread = pi.hThread;
        }
        else
        {
            msgprompt(L"Failed to start game");
            return 1;
        }
    }
    else
    {
        msgprompt(L"The game was not found on its default path of " GAME_PATH L"! Please check the default path used by the launcher and modify it based off your needs. For further information check the README of the repo.");
        return 0;
    }

    if (game_handle == nullptr)
    {
        msgprompt(L"Invalid handle to game process!");
        return 1;
    }

    if (!utils::remote_loadlibrary(game_handle, utils::get_full_path(L"gsf-client.dll")))
    {
        msgprompt(L"Failed to load gsf-client to Genshin Impact!");
        return 1;
    }

    return 0;
}