#pragma once

#include "File.hpp"

namespace OK
{
class FileVec final
{
public:
	// TODO: Add proper directory listing for multiple inputs with the file names shown
	FileVec(int argc, char** argv, ParsedOptions options) :
		m_parsed_options {options}, m_argc_mode {true}
	{
		m_files.reserve(static_cast<std::size_t>(argc - 1));
		for(int i = 1; i < argc; ++i)
		{
			fs::path file_path {argv[i]};
			if(!fs::exists(file_path))
			{
				fmt::print(stderr,
						   "    {}File or directory not found '{}'{}\n",
						   OK::Color::RED,
						   argv[i],
						   OK::Color::RESET);
				m_return_value = 2;
				continue;
			}
			m_files.emplace_back(argv[i]);
			handle_lengths();
		}

		std::sort(m_files.begin(), m_files.end());
	}

	FileVec(File&& input_file, ParsedOptions options) : m_parsed_options {options}
	{
		m_files.emplace_back(std::move(input_file));
	}

	FileVec(ParsedOptions options) : m_parsed_options {options} { m_files.reserve(4ULL); }

	FileVec(File&& input_file, ssize_t num_of_files, ParsedOptions options) :
		m_parsed_options {options}
	{
		m_files.reserve(static_cast<std::size_t>(num_of_files));

		if(options.all)
		{
			for(const auto& path: fs::directory_iterator(input_file.path()))
			{
				m_files.emplace_back(path);
				handle_lengths();
			}
		}
		else
		{
			for(const auto& path: fs::directory_iterator(input_file.path()))
			{
				if(path.path().filename().c_str()[0] != '.')
					m_files.emplace_back(path);
				handle_lengths();
			}
		}

		std::sort(m_files.begin(), m_files.end());
	}

	auto begin() noexcept { return m_files.begin(); }
	auto end() noexcept { return m_files.end(); }
	auto cbegin() const noexcept { return m_files.cbegin(); }
	auto cend() const noexcept { return m_files.cend(); }

	void push(File&& file) const
	{
		m_files.emplace_back(std::move(file));
		handle_lengths();
	}

	auto return_value() const noexcept { return m_return_value; }

	void print() const;

private:
	mutable std::vector<File> m_files {};
	mutable std::size_t m_longest_file_size {1ULL};
	mutable std::size_t m_longest_username {1ULL};
	mutable std::size_t m_longest_groupname {1ULL};
	const ParsedOptions m_parsed_options;
	int m_return_value {0};
	const bool m_argc_mode {false};

	void print_long() const;
	void print_columnal() const;
	void print_one_liner() const;
	void print_argc() const;
	void print_argc_directory(File&& dir) const;

	void handle_lengths() const
	{
		const auto& back = m_files.back();
		m_longest_file_size = std::max(m_longest_file_size, back.get_size_digit_count());
		m_longest_username = std::max(m_longest_username, back.username().size());
		m_longest_groupname = std::max(m_longest_groupname, back.groupname().size());
	}
};
}	 // namespace OK
