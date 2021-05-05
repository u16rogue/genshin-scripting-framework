#include <console.h>
#include <Windows.h>

static HANDLE stdout_handle = nullptr;

con::color::color( con::colors foreground, con::colors background )
	: fg_color(static_cast<std::uint8_t>(foreground)), bg_color(static_cast<std::uint8_t>(background))
{
}

bool con::init()
{
	stdout_handle = GetStdHandle( STD_OUTPUT_HANDLE );
	con::print();
	return stdout_handle != nullptr;
}

void con::print( void )
{
	con::print(con::color());
}

void con::print( con::color val )
{
	SetConsoleTextAttribute(stdout_handle, val.full);
}

void con::print( con::colors val )
{
	con::print( con::color(val) );
}
