#include "File.hpp"

#include <cwchar>
#include <fmt/format.h>

using namespace fmt::literals;

namespace OK
{
File::File(const fs::path file_path) :
	m_file_path {file_path}, m_file_type {fs::symlink_status(m_file_path).type()}
{
	if(fs::is_directory(m_file_path))
	{
		m_file_size = 4096ULL;
	}
	else
	{
		if(fs::exists(m_file_path))
			m_file_size = fs::file_size(m_file_path);
	}

	m_file_name = file_path.string();
	m_file_name.erase(0ULL, m_file_name.find_last_of(L'/') + 1ULL);
	m_extension = get_ext_from_filename(m_file_name);
	handle_icon_and_color();
}

std::string File::to_string(const bool long_listing) const noexcept
{
	if(!long_listing)
	{
		return fmt::format("{}{}{}{}{}", m_color, m_icon, m_file_name, Color::RESET, m_indicator);
	}
	// TODO: Add long listing format with user group info and time and perms
	return fmt::format("LMAO");
}

// Thanks https://stackoverflow.com/a/18850689, @user2781185
std::size_t mb_strlen(const std::string& str)
{
	std::size_t curr_length = 0ULL;
	const char* c_str = str.c_str();
	std::size_t char_count = 0ULL;
	const std::size_t max_length = str.size();
	while(curr_length < max_length)
	{
		curr_length += std::mblen(&c_str[curr_length], max_length - curr_length);
		char_count += 1;
	}
	return char_count;
}

std::size_t File::string_length(const bool long_listing) const noexcept
{
	if(!long_listing)
	{
		std::size_t total_length = 0ULL;
		total_length += 2ULL;
		total_length += mb_strlen(m_file_name);
		total_length += m_indicator.size();
		return total_length;
	}

	return 0ULL;
}

void File::handle_icon_and_color() noexcept
{
	const auto icon_opt = get_icon_from_ext(m_extension);
	m_icon = icon_opt.value_or(default_file_icon);
	switch(m_file_type)
	{
		case fs::file_type::directory:
		{
			m_icon = icon_opt.value_or(default_directory_icon);
			m_color = Color::DIR;
			m_indicator = "/";
			return;
		}
		case fs::file_type::regular:
		{
			m_color = Color::FILE;
			// TODO: Check for executable perm and put an asterisk here
			if((fs::status(m_file_path).permissions() & fs::perms::owner_exec) != fs::perms::none)
				m_indicator = "*";
			return;
		}
		case fs::file_type::symlink:
		{
			// This only does one level of following, what if the file that's linked to is a link?
			const auto followed_link = fs::read_symlink(m_file_path);
			if(!fs::exists(followed_link))
			{
				m_color = Color::BRKN_LINK;
			}
			else if(fs::is_directory(followed_link))
			{
				m_color = Color::DIR_LINK;
				m_icon = icon_opt.value_or(default_directory_icon);
			}
			else	// Not handling if it links to sockets or what not
			{
				m_color = Color::REG_LINK;
			}
			m_indicator = "@";
			return;
		}
		case fs::file_type::socket:
		{
			m_color = Color::SOCK;
			m_indicator = "=";
			return;
		}
		case fs::file_type::fifo:
		{
			m_color = Color::PIPE;
			m_indicator = "|";
			return;
		}
	}
}

}	 // namespace OK
