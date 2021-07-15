#include <Windows.h>
#include <filesystem>
#include <loadlibrary.h>
#include <winapi_helper.h>

// ------------------------------------------------------------------------

#define GAME_DIRECTORY \
    L"C:\\Program Files\\Genshin Impact\\Genshin Impact game"

#define GAME_PATH \
    GAME_DIRECTORY L"\\GenshinImpact.exe"

#define MSG_TITLE \
    L"gsf-launcher"

#define GAME_WND_CLASS \
    L"UnityWndClass"

#define GAME_WND_TITLE \
    L"Genshin Impact"

// ------------------------------------------------------------------------

void msgprompt(const wchar_t *msg)
{
    MessageBoxW(nullptr, msg, MSG_TITLE, MB_OK);
}

bool launch_game(HANDLE &handle_out)
{
    if (std::filesystem::exists(GAME_PATH))
    {
        STARTUPINFO         si{ sizeof(si) };
        PROCESS_INFORMATION pi{ NULL };

        if (CreateProcessW(GAME_PATH, nullptr, nullptr, nullptr, FALSE, NULL, nullptr, GAME_DIRECTORY, &si, &pi))
        {
            CloseHandle(pi.hThread);
            handle_out = pi.hProcess;
            return true;
        }
        else
        {
            msgprompt(L"Failed to start game");
            return false;
        }
    }
    else
    {
        msgprompt(L"The game was not found on its default path of " GAME_PATH L"! Please check the default path used by the launcher and modify it based off your needs. For further information check the README of the repo.");
        return false;
    }
}

bool attach_to_game(HANDLE &handle_out)
{
    HWND game_wnd = FindWindowW(GAME_WND_CLASS, GAME_WND_TITLE);
    if (!game_wnd)
        return false;

    DWORD game_pid = 0;
    GetWindowThreadProcessId(game_wnd, &game_pid);

    if (game_pid == 0)
        return false;

    handle_out = OpenProcess(PROCESS_ALL_ACCESS, FALSE, game_pid);
    return handle_out != nullptr;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
    HANDLE game_handle = nullptr;

    attach_to_game(game_handle);

    if (!game_handle && !launch_game(game_handle))
        return 1;

    if (!utils::remote_loadlibrary(game_handle, utils::get_full_path(L"gsf-client.dll")))
    {
        msgprompt(L"Failed to load gsf-client to Genshin Impact!");
        return 1;
    }

    CloseHandle(game_handle);

    return 0;
}