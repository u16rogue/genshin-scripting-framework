#pragma once

#include <string>
#include <memory>
#include <vector>

#pragma warning (disable: 26451, 26820, 26812, 28020, 26495, 4244)
#include <chaiscript.hpp>
#pragma warning (default: 26451, 26820, 26812, 28020, 26495, 4244)

namespace gsf
{
	/// <summary>
	/// An instance of a single chaiscript script containing its own chaiscript::ChaiScript instance.
	/// </summary>
	class cs_script
	{
	public:
		cs_script(const std::wstring_view filepath_);

		/// <summary>
		/// Loads the script specified by the filepath, creates an instance of ChaiScript, and then evaluates the loaded script.
		/// </summary>
		/// <returns>Returns true if all operations were successful and the script was evaluated with no errors, otherwise returns false.</returns>
		bool load();

		/// <summary>
		/// Unloads the current script. Also handles de-allocation of allocated memory, termination of threads, disposing of callbacks, etc...
		/// </summary>
		/// <returns>Returns true if all operations were successful and the script was evaluated with no errors, otherwise returns false.</returns>
		bool unload();

		/// <returns>Returns the identifier of the script.</returns>
		const int get_id();

	private:
		int id = 0;
		const std::wstring filepath;
		std::vector<const void *> allocated_memory;
		std::unique_ptr<chaiscript::ChaiScript> chai_inst;

	public:
		class factory
		{
			static bool add(std::wstring_view filepath_entry);
			static bool remove(const int id);
		};
		
		inline static std::vector<gsf::cs_script *> instances;
	};
}