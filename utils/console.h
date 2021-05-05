#pragma once

#include <cstdint>
#include <iostream>

namespace con
{
	enum class colors
	{
		BLACK,
		BLUE,
		GREEN,
		AQUA,
		RED,
		PURPLE,
		YELLOW,
		WHITE,
		GRAY,
		LBLUE,
		LGREEN,
		LAQUA,
		LRED,
		LPURPLE,
		LYELLOW,
		BWHITE,
	};

	struct color
	{
		con::color(con::colors foreground = con::colors::BWHITE, con::colors background = con::colors::BLACK);

		union
		{
			std::uint16_t full;
			std::uint8_t half[2];
			struct
			{
				std::uint8_t fg_color;
				std::uint8_t bg_color;
			};
		};
	};

	bool init();

	void print( void );
	void print( con::color val );
	void print( con::colors val );

	template <typename T>
	void print( T val )
	{
		std::wcout << val;
		con::print();
	}

	template <typename T, typename ...targs>
	void print( T val, targs... arg )
	{
		con::print( val );
		con::print( arg... );
	}

	template <typename ...targs>
	void print( con::color val, targs... arg )
	{
		con::print( val );
		con::print( arg... );
	}

	template <typename ...targs>
	void print( con::colors val, targs... arg )
	{
		con::print( val );
		con::print( arg... );
	}
}