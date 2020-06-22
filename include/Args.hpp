#pragma once

#include "cxxopts.hpp"

#include <fmt/core.h>
#include <optional>

namespace OK
{
// TODO: Make a settings/configurations struct with these options
struct ParsedOptions
{
	const bool all : 1;
	const bool human : 1;
	const bool long_listing : 1;
	const bool color : 1;
	const bool no_color : 1;
	const bool reverse : 1;
	const bool one_line : 1;
	const bool dir : 1;
};

class Args final
{
public:
	Args()
	{
		m_options.add_options()("H,help",
								"Print this screen")("a,all", "Show hidden files, except . and ..")(
			"h,human", "Human readable sizes, SI units (1KB=1000B)")(
			"l,long", "Long listing with size, ownership, permissions and time info")(
			"c,color", "Enable colorful printing (default)")(
			"C,no-color", "Disable colorful printing, will disable if STDOUT is not a tty")(
			"r,reverse", "Reverse the printing order")(
			"1,one-line", "Print a single file on each row, not --long")(
			"d,dir", "Don't go into a directory, just print it");
	}

	[[nodiscard]] std::optional<ParsedOptions> parse(int& argc, char**& argv)
	{
		try
		{
			const auto result = m_options.parse(argc, argv);
			if(result.count("help"))
			{
				fmt::print(stderr, "{}", m_options.help());
				return std::nullopt;
			}

			return ParsedOptions {0UL != result.count("all"),
								  0UL != result.count("human"),
								  0UL != result.count("long"),
								  0UL != result.count("color"),
								  0UL != result.count("no-color"),
								  0UL != result.count("reverse"),
								  0UL != result.count("one-line"),
								  0UL != result.count("dir")};
		}
		catch(const cxxopts::OptionParseException& e)
		{
			fmt::print(stderr, "{}\n", e.what());
			return std::nullopt;
		}
	}

private:
	cxxopts::Options m_options {"list", "A colorful ls clone that works with NerdFonts"};
};

}	 // namespace OK
