#include "FileVec.hpp"

#include <numeric>
#include <sys/ioctl.h>
#include <unistd.h>

namespace OK
{
void FileVec::print_long(ParsedOptions options) const
{
	for(const auto& file: m_files)
	{
		fmt::print(FMT_STRING("{}"), file.long_name_to_string(options));
	}
}

uint16_t get_term_width() noexcept
{
	struct winsize ws;
	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != 0)
		return 80U;
	return ws.ws_col;
}

void FileVec::print_columnal() const
{
	const auto term_width = get_term_width();
	std::size_t total_file_name_length = m_files.front().string_length();

	const auto longest_string_file =
		std::max_element(m_files.cbegin(),
						 m_files.cend(),
						 [&total_file_name_length](const auto& lhs, const auto& rhs) {
							 total_file_name_length += rhs.string_length();
							 return lhs.string_length() < rhs.string_length();
						 });

	const auto longest_string_length = longest_string_file->string_length();

	if(longest_string_length > term_width)
	{
		// One liner
		print_one_liner();
		return;
	}

	fmt::print("    ");
	const auto column_count = term_width / longest_string_length;
	for(std::size_t i = 0ULL; i < column_count; ++i)
	{
		const auto& file_string = m_files[i].icon_and_color_filename();
		const auto width = longest_string_file->icon_and_color_filename_length();

		fmt::print(FMT_STRING("{:{}}"), file_string, width + 4ULL);
	}

	fmt::print("\n");
}

void FileVec::print_one_liner() const
{
	for(const auto& file: m_files)
		fmt::print("    {}\n", file.icon_and_color_filename());
}

void FileVec::print(ParsedOptions options) const
{
	if(options.long_listing)
		print_long(options);
	else if(options.one_line)

		print_one_liner();
	else
		print_columnal();
}

}	 // namespace OK
