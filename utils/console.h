#pragma once

#include <cstdint>
#include <iostream>
#include <Windows.h>

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
		color(con::colors foreground = con::colors::BWHITE, con::colors background = con::colors::BLACK);

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

	bool is_focused();
	bool is_allocated();

	HWND   get_window();
	HANDLE get_std();

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

	class log
	{
		struct con_point_t
		{
			short x, y;
		};

	public:

		template <typename T>
		log(const T *text, bool newline = true, const wchar_t def_status[10] = L" WAIT... ")
		{
			if (newline)
				std::cout << '\n';

			CONSOLE_SCREEN_BUFFER_INFO csbi{};
			GetConsoleScreenBufferInfo(con::get_std(), &csbi);
			this->status_point.x = csbi.dwCursorPosition.X + 1;
			this->status_point.y = csbi.dwCursorPosition.Y;

			con::print(con::colors::BWHITE, "[", con::colors::GRAY, def_status, con::colors::BWHITE, "] ", text);
		}

		void status(const wchar_t text[10], con::colors txtcol = con::colors::WHITE);
		void success();
		void error();

		bool check(bool result);

		template <typename T>
		static void out( T text )
		{
			con::print(con::colors::BWHITE, "\n[   LOG   ] ", text);
		}

		template <typename T>
		static void warn( T text )
		{
			con::print( con::colors::BWHITE, "\n[", con::colors::LYELLOW, " WARNING ", con::colors::BWHITE, "] ", text );
		}

		template <typename T>
		static void critical( T text )
		{
			con::print( con::colors::BWHITE, "\n[", con::colors::LRED, " CRITERR ", con::colors::BWHITE, "] ", text );
		}

	private:
		con_point_t status_point = {};
	};
}