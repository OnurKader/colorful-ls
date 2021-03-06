#include "FileVec.hpp"

#include "Utils.hpp"

#include <numeric>
#include <sys/ioctl.h>
#include <unistd.h>

namespace OK
{
FileVec::FileVec(const std::vector<std::string>& filenames, const ParsedOptions& options) :
	// m_files {filenames.cbegin(), filenames.cend()}, m_parsed_options {options}
	m_parsed_options {options},
	m_argc_mode {true}
{
	m_files.reserve(filenames.size());
	for(auto&& filename: filenames)
	{
		if(!fs::exists(filename))
		{
			fmt::print(stderr,
					   "    {}File or directory not found '{}'{}\n",
					   OK::Color::RED,
					   filename,
					   OK::Color::RESET);
			m_return_value = 1;
			continue;
		}
		m_files.emplace_back(filename);
		handle_lengths();
	}

	std::sort(m_files.begin(), m_files.end());
}

void FileVec::print_long()
{
	if(m_parsed_options.human)
	{
		if(m_parsed_options.kibi)
			m_longest_file_size = 5ULL;
		else
			m_longest_file_size = 4ULL;
	}

	for(auto&& file: m_files)
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

	// If output isn't a tty, or something bad happened in ioctl, return 80 columns
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
		print_one_liner();
		return;
	}

	std::size_t offset = 4UL;
	if(term_width > longest_string_length)
		offset = term_width % longest_string_length / (term_width / longest_string_length);

	const auto column_count = longest_string_length + std::max(offset, 4UL) + 4UL;
	std::size_t current_cursor_pos = 4UL;

	fmt::print("offset: {}, longest_string_length: {}\nterm_width: {}, column_count: "
			   "{}\nlongest_string_file: {}\n",
			   offset,
			   longest_string_length,
			   term_width,
			   column_count,
			   longest_string_file->name());

	fmt::print("    ");
	for(std::size_t i = 0UL; i < m_files.size(); ++i)
	{
		const auto& file = m_files[i];
		const auto& file_string = file.icon_and_color_filename();
		const auto width = longest_string_file->icon_and_color_filename_length() + 4UL;

		current_cursor_pos += column_count;
		const bool overflowed_line = (current_cursor_pos + column_count - 8UL >= term_width);
		if(overflowed_line)
		{
			if(i == m_files.size() - 1UL)
				fmt::print("{}\n", file_string);
			else
				fmt::print("{}\n    ", file_string);
			// FIXME: Change the right 4UL's to constexpr variables, or adjustable from commandline
			current_cursor_pos = 4UL;
		}
		else
		{
			fmt::print("{:{}}", file_string, width);
		}
	}
	if(current_cursor_pos > 4UL)
		fmt::print("\n");
}

void FileVec::print_one_liner() const
{
	for(const auto& file: m_files)
		fmt::print("    {}\n", file.icon_and_color_filename());
}

void FileVec::print_argc_directory(File&& dir) const
{
	// Do a custom formatting of the color, the icon and the path_name. No indicator, maybe the
	// symlink colors can be ignored, just OK::Color::DIR and the path().c_str(). Who Knows...
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

	FileVec dir_vec {std::move(dir), num_of_files_in_directory, m_parsed_options};
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

void FileVec::print()
{
	if(m_files.empty())
		return;

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
