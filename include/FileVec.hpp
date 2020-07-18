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

		for(const auto& path: fs::directory_iterator(input_file.path()))
		{
			if(all_files)
			{
				m_files.emplace_back(path);
			}
			else
			{
				if(path.path().filename().c_str()[0] != '.')
					m_files.emplace_back(path);
			}
		}

		std::sort(m_files.begin(), m_files.end());
	}

	auto begin() { return m_files.begin(); }
	auto end() { return m_files.end(); }
	auto cbegin() const { return m_files.cbegin(); }
	auto cend() const { return m_files.cend(); }

	void print(ParsedOptions) const;

private:
	std::vector<File> m_files;

	void print_long(ParsedOptions) const;
	void print_columnal() const;
	void print_one_liner() const;
};
}	 // namespace OK
