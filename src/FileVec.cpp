#include "FileVec.hpp"

#include "Utils.hpp"

#include <numeric>
#include <sys/ioctl.h>
#include <unistd.h>

namespace OK
{
void FileVec::print_long() const
{
	if(m_parsed_options.human)
	{
		if(m_parsed_options.kibi)
			m_longest_file_size = 5ULL;
		else
			m_longest_file_size = 4ULL;
	}

	for(const auto& file: m_files)
	{
		fmt::print(
			"{}",
			file.long_name_to_string(
				m_parsed_options, m_longest_file_size, m_longest_username, m_longest_groupname));
	}
}

inline uint16_t get_term_width() noexcept
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

void FileVec::print_argc_directory(File&& dir) const
{
	// First print the directory name, Color shall follow later.
	fmt::print("{}:\n", dir.icon_and_color_filename());

	const auto num_of_files_in_directory = std::distance(
		std::filesystem::directory_iterator {dir.path()}, std::filesystem::directory_iterator {});

	if(num_of_files_in_directory == 0L)
	{
		fmt::print(stderr,
				   "    {}Nothing to show here...{}\n\n",
				   OK::Color::rgb(229, 195, 38),
				   OK::Color::RESET);
		return;
	}

	const FileVec dir_vec {std::move(dir), num_of_files_in_directory, m_parsed_options};
	dir_vec.print();
	fmt::print("\n");
}

void FileVec::print_argc() const
{
	FileVec regular_file_vec {m_parsed_options};
	for(auto& file: m_files)
	{
		const bool file_is_directory = File::is_directory(file);

		if(file_is_directory)
			print_argc_directory(std::move(file));
		else
			regular_file_vec.push(std::move(file));
		// print_argc_file(std::move(file));
	}

	regular_file_vec.print();
}

void FileVec::print() const
{
	if(m_argc_mode)
	{
		print_argc();
	}
	else
	{
		if(m_parsed_options.long_listing)
			print_long();
		else if(m_parsed_options.one_line)
			print_one_liner();
		else
			print_columnal();
	}
}

}	 // namespace OK
