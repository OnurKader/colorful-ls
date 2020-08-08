#pragma once

#include "ProgramOptions.hxx"

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <optional>

namespace OK
{
struct ParsedOptions
{
	const bool all;
	const bool human;
	const bool kibi;
	const bool long_listing;
	const bool color;
	const bool no_color;
	const bool reverse;
	const bool one_line;
	const bool dir;
	const bool help;
};

class Args final
{
public:
	[[nodiscard]] std::optional<ParsedOptions> parse(const int argc, char** argv)
	{
		const auto& all =
			m_parser["all"].abbreviation('a').description("Show hidden files, except . and ..");
		const auto& human = m_parser["human"].abbreviation('h').description(
			"Human readable sizes SI Units by default (1K = 1000)");
		const auto& kibi = m_parser["kibi"].abbreviation('k').description(
			"Use Kibi-Byte system (1Ki = 1024) [Requires -h]");
		const auto& long_listing = m_parser["long"].abbreviation('l').description(
			"Long listing with size, owners, perms and time");
		const auto& color = m_parser["color"].abbreviation('c').description(
			"Force enable colors (enabled by default)");
		const auto& no_color = m_parser["no-color"].abbreviation('C').description(
			"Disable colorful printing (Default for non-tty files)");
		const auto& reverse =
			m_parser["reverse"].abbreviation('r').description("Reverse the order of printing");
		const auto& one_line =
			m_parser["one-line"].abbreviation('1').description("Print a single file on each line");
		const auto& dir = m_parser["dir"].abbreviation('d').description(
			"Don't go into a directory, just print it like a file");

		const auto& help = m_parser["help"].abbreviation('?').description("Print this help screen");

		// Handle positional filenames
		m_positionals.reserve(4UL);

		m_parser[""].description("Filenames to print").bind(m_positionals);

		if(!m_parser(argc, argv))
		{
			fmt::print(stderr, "Error during CLI parsing\n");
			return std::nullopt;
		}

		if(help.was_set())
		{
			fmt::print("{}\n", m_parser);
			return std::nullopt;
		}

		for(auto&& str: m_positionals)
			fmt::print("Positional: {}\n", str);

		return ParsedOptions {
			all.was_set(),
			human.was_set(),
			kibi.was_set(),
			long_listing.was_set(),
			color.was_set(),
			no_color.was_set(),
			reverse.was_set(),
			one_line.was_set(),
			dir.was_set(),
			help.was_set(),
		};
	}

	const std::vector<std::string>& positionals() const noexcept { return m_positionals; }

private:
	po::parser m_parser {};
	std::vector<std::string> m_positionals {};
};

}	 // namespace OK
