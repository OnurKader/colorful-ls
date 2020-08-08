#pragma once

#include "lyra/lyra.hpp"

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <optional>

namespace OK
{
// Bit-fields just make the code ugly, just do bit masking enums
struct ParsedOptions
{
	const bool all : 1;
	const bool human : 1;
	const bool kibi : 1;
	const bool long_listing : 1;
	const bool color : 1;
	const bool no_color : 1;
	const bool reverse : 1;
	const bool one_line : 1;
	const bool dir : 1;
	const bool help : 1;
};

class Args final
{
public:
	[[nodiscard]] std::optional<ParsedOptions> parse(const int argc, char* const* const argv)
	{
		bool all = false;
		bool human = false;
		bool kibi = false;
		bool long_listing = false;
		bool color = false;
		bool no_color = false;
		bool reverse = false;
		bool one_line = false;
		bool dir = false;
		bool help = false;

		m_options |= lyra::opt(all)["-a"]["--all"]("Show hidden files, except . and ..").optional();
		m_options |= lyra::opt(human)["-h"]["--human"](
						 "Human readable sizes SI Units by default (1K = 1000)")
						 .optional();
		m_options |=
			lyra::opt(kibi)["-k"]["--kibi"]("Use Kibi-Byte system (1Ki = 1024). Requires -h")
				.optional();
		m_options |= lyra::opt(long_listing)["-l"]["--long"](
						 "Long listing with size, owners, perms and time")
						 .optional();
		m_options |= lyra::opt(color)["-c"]["--color"]("Force enable colors").optional();
		m_options |= lyra::opt(no_color)["-C"]["--no-color"](
						 "Disable colorful printing (Default for non-tty files)")
						 .optional();
		m_options |=
			lyra::opt(reverse)["-r"]["--reverse"]("Reverse the order of printing").optional();
		m_options |=
			lyra::opt(one_line)["-1"]["--one-line"]("Print a single file on each line").optional();
		m_options |=
			lyra::opt(dir)["-d"]["--dir"]("Don't go into a directory, just print it like a file")
				.optional();
		m_options |= lyra::opt(help)["-H"]["-?"]["--help"]("Print this screen").optional();

		m_options |= lyra::arg(m_positionals, "files...")("Filenames to print").optional();

		const auto results = m_options.parse({argc, argv});
		if(!results)
		{
			fmt::print(stderr, "Error during CLI parsing: {}\n", results.errorMessage());
			return std::nullopt;
		}

		if(help)
		{
			fmt::print("{}\n", m_options);
			return std::nullopt;
		}

		return ParsedOptions {
			all, human, kibi, long_listing, color, no_color, reverse, one_line, dir, help};
	}

private:
	// cxxopts::Options m_options {"list", "A colorful ls clone that works with NerdFonts"};
	lyra::cli_parser m_options {};
	std::vector<std::string> m_positionals {};
};

}	 // namespace OK
