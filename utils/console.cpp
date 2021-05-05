#include <console.h>
#include <Windows.h>

static HANDLE stdout_handle = nullptr;
static HWND   window_handle = nullptr;

con::color::color( con::colors foreground, con::colors background )
	: fg_color(static_cast<std::uint8_t>(foreground)), bg_color(static_cast<std::uint8_t>(background))
{
}

bool con::init()
{
	if ( AllocConsole() )
	{
		FILE *file_ptr;
		freopen_s( &file_ptr, "CONOUT$", "w", stdout );
		freopen_s( &file_ptr, "CONOUT$", "w", stderr );
		freopen_s( &file_ptr, "CONIN$", "r", stdin );
	}

	window_handle = GetConsoleWindow();
	stdout_handle = GetStdHandle( STD_OUTPUT_HANDLE );

	if ( stdout_handle && window_handle )
	{
		con::print();
		return true;
	}

	#if _DEBUG
	DebugBreak();
	#endif
	return false;
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
	con::print( con::color(val) );
}
