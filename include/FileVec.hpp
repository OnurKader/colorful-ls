#pragma once

#include "File.hpp"

namespace OK
{
class FileVec final
{
public:
	FileVec(File&& input_file, ssize_t num_of_files, bool all_files)
	{
		m_files.reserve(static_cast<std::size_t>(num_of_files));

		if(all_files)
		{
			for(const auto& path: fs::directory_iterator(input_file.path()))
			{
				m_files.emplace_back(path);
				const auto& back = m_files.back();
				m_longest_file_size = std::max(m_longest_file_size, back.get_size_digit_count());
				m_longest_username = std::max(m_longest_username, back.username().size());
				m_longest_groupname = std::max(m_longest_groupname, back.groupname().size());
			}
		}
		else
		{
			for(const auto& path: fs::directory_iterator(input_file.path()))
			{
				if(path.path().filename().c_str()[0] != '.')
					m_files.emplace_back(path);
				m_longest_file_size =
					std::max(m_longest_file_size, m_files.back().get_size_digit_count());
			}
		}

		std::sort(m_files.begin(), m_files.end());
	}

	auto begin() noexcept { return m_files.begin(); }
	auto end() noexcept { return m_files.end(); }
	auto cbegin() const noexcept { return m_files.cbegin(); }
	auto cend() const noexcept { return m_files.cend(); }

	void print(ParsedOptions) const;

private:
	std::vector<File> m_files {};
	mutable std::size_t m_longest_file_size {1ULL};
	mutable std::size_t m_longest_username {1ULL};
	mutable std::size_t m_longest_groupname {1ULL};

	void print_long(ParsedOptions) const;
	void print_columnal() const;
	void print_one_liner() const;
};
}	 // namespace OK
