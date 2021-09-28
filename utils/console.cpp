#include "console.h"

static HANDLE stdout_handle  = nullptr;
static HWND   window_handle  = nullptr;
static bool   flag_allocated = false;

con::color::color( con::colors foreground, con::colors background )
	: fg_color(static_cast<std::uint8_t>(foreground)), bg_color(static_cast<std::uint8_t>(background))
{
}

bool con::init()
{
	if ( AllocConsole() )
	{
		FILE *file_ptr;
		_wfreopen_s( &file_ptr, L"CONOUT$", L"w", stdout );
		_wfreopen_s( &file_ptr, L"CONOUT$", L"w", stderr );
		_wfreopen_s( &file_ptr, L"CONIN$",  L"r", stdin  );

		flag_allocated = true;
	}
	else
	{
		flag_allocated = false;
	}

	window_handle = GetConsoleWindow();
	stdout_handle = GetStdHandle( STD_OUTPUT_HANDLE );

	if ( stdout_handle && window_handle )
	{
		con::print();
		return true;
	}

	return false;
}

bool con::is_focused()
{
	return window_handle == GetForegroundWindow();
}

bool con::is_allocated()
{
	return flag_allocated;
}

HWND con::get_window()
{
	return window_handle;
}

HANDLE con::get_std()
{
	return stdout_handle;
}

void con::print( void )
{
	con::print( con::color() );
}

void con::print( con::color val )
{
	SetConsoleTextAttribute( stdout_handle, val.full );
}

void con::print( con::colors val )
{
	con::print( con::color( val ) );
}

void con::log::status(const wchar_t text[10], con::colors txtcol)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi{};
	GetConsoleScreenBufferInfo(stdout_handle, &csbi);
	SetConsoleCursorPosition(stdout_handle, { this->status_point.x, this->status_point.y });
	con::print(txtcol, text);
	SetConsoleCursorPosition(stdout_handle, csbi.dwCursorPosition);
}

void con::log::success()
{
	this->status(L" SUCCESS ", con::colors::LGREEN);
}

void con::log::error()
{
	this->status(L"  ERROR  ", con::colors::LRED);
}

bool con::log::check(bool result)
{
	if (result)
		this->success();
	else
		this->error();

	return result;
}
