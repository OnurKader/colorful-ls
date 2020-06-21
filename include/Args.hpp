#pragma once

#include "cxxopts.hpp"

#include <fmt/core.h>
#include <optional>

namespace OK
{
class Args final
{
public:
	Args()
	{
		m_options.add_options()("H,help",
								"Print this screen")("a,all", "Show hidden files, except . and ..")(
			"h,human", "Human readable sizes, SI units (1KB=1000B)")(
			"l,long", "Long listing with size, ownership, permissions and time info");
	}

	[[nodiscard]] std::optional<cxxopts::ParseResult> parse(int& argc, char**& argv)
	{
		try
		{
			const auto result = m_options.parse(argc, argv);
			if(result.count("help"))
			{
				fmt::print(stderr, "{}", m_options.help());
				return std::nullopt;
			}

			return result;
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
