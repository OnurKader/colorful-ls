#include "FileVec.hpp"

#include <numeric>
#include <sys/ioctl.h>
#include <unistd.h>

namespace OK
{
void FileVec::print_long(ParsedOptions options) const
{
	if(options.human)
		m_longest_file_size = 4ULL;

	for(const auto& file: m_files)
	{
		fmt::print("{}",
				   file.long_name_to_string(
					   options, m_longest_file_size, m_longest_username, m_longest_groupname));
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
	std::size_t total_file_name_length = m_files.front().string_length() + 4ULL;

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

	std::size_t offset = 4ULL;
	if(term_width > longest_string_length)
		offset = term_width % longest_string_length / (term_width / longest_string_length);

	const auto column_count = longest_string_length + (offset > 4ULL ? offset : 4ULL) + 4ULL;
	std::size_t current_cursor_pos = 4ULL;

	fmt::print("    ");
	for(std::size_t i = 0ULL; i < m_files.size(); ++i)
	{
		const auto& file = m_files[i];
		const auto& file_string = file.icon_and_color_filename();
		const auto width = longest_string_file->icon_and_color_filename_length() + 4ULL;

		current_cursor_pos += column_count;
		const bool overflowed_line = (current_cursor_pos + column_count) >= term_width;
		if(overflowed_line)
		{
			if(i == m_files.size() - 1ULL)
				fmt::print("{}\n", file_string);
			else
				fmt::print("{}\n    ", file_string);
			current_cursor_pos = 4ULL;
		}
		else
		{
			fmt::print("{:{}}", file_string, width);
		}
	}
	if(current_cursor_pos > 4ULL)
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
