#include "File.hpp"

#include <algorithm>
#include <cwchar>
#include <fmt/chrono.h>
#include <fmt/format.h>

// Sorry for this!
#include <sys/stat.h>

using namespace fmt::literals;

namespace OK
{
File::File(const fs::path file_path) :
	m_file_path {file_path},
	m_file_type {fs::symlink_status(m_file_path).type()},
	m_file_name {file_path.string()}
{
	if(fs::is_directory(m_file_path))
	{
		m_file_size = 4096ULL;
	}
	else if(fs::exists(m_file_path))
	{
		// We're making File's, how can the path not exist
		m_file_size = fs::file_size(m_file_path);
	}

	m_file_name.erase(0ULL, m_file_name.find_last_of('/') + 1ULL);
	m_extension = get_ext_from_filename(m_file_name);
	handle_icon_and_color();
}

File::File(File&& other) :
	m_file_path {std::move(other.m_file_path)},
	m_file_type {std::move(other.m_file_type)},
	m_file_name {std::move(other.m_file_name)},
	m_file_size {std::move(other.m_file_size)},
	m_extension {std::move(other.m_extension)},
	m_icon {std::move(other.m_icon)},
	m_color {std::move(other.m_color)},
	m_indicator {std::move(other.m_indicator)}
{
	// MAYBE: Default this constructor?
}

File& File::operator=(File&& other)
{
	m_file_path = std::move(other.m_file_path);
	m_file_type = std::move(other.m_file_type);
	m_file_size = std::move(other.m_file_size);
	m_file_name = std::move(other.m_file_name);
	m_extension = std::move(other.m_extension);
	m_color = std::move(other.m_color);
	m_icon = std::move(other.m_icon);
	m_indicator = std::move(other.m_indicator);

	return *this;
}

inline std::wstring mb_to_wstring(const std::string& mb_str)
{
	std::wstring temp_wide(mb_str.size(), L'\0');
	// mbtowcs returns length of wstring, should we erase the last parts? Nah.
	std::mbstowcs(temp_wide.data(), mb_str.data(), mb_str.size());
	return temp_wide;
}

inline void ws_tolower(std::wstring& wstr)
{
	std::transform(wstr.begin(), wstr.end(), wstr.begin(), [loc = std::locale {""}](const auto& c) {
		return std::tolower(c, loc);
	});
}

inline std::string w_to_mbstring(const std::wstring& wstr)
{
	std::string temp_mb(wstr.size(), '\0');
	std::wcstombs(temp_mb.data(), wstr.data(), wstr.size());
	return temp_mb;
}

inline std::string mb_lower(const std::string& mb_str)
{
	std::wstring temp = mb_to_wstring(mb_str);
	ws_tolower(temp);
	return w_to_mbstring(temp);
}

// FIXME: Change this to check properly
bool File::operator<(const File& other) const
{
	// Do checks for symlinks
	using ft = fs::file_type;
	ft this_type = m_file_type;
	if(fs::is_symlink(m_file_path))
	{
		const auto followed_stat = fs::status(m_file_path);
		if(fs::exists(followed_stat))
			this_type = followed_stat.type();
	}

	if(this_type == ft::directory && other.m_file_type != ft::directory)
		return true;
	else if(this_type != ft::directory && other.m_file_type == ft::directory)
		return false;
	else
		return mb_lower(m_file_name) < mb_lower(other.m_file_name);
}

std::string File::icon_and_color_filename() const noexcept
{
	return fmt::format("{}{}{}{}{}", m_color, m_icon, m_file_name, Color::RESET, m_indicator);
}

std::string File::to_string(const ParsedOptions po) const noexcept
{
	if(!po.long_listing)
	{
		return icon_and_color_filename();
	}

	// Because I'm doing {:>5} for size, this can't be inside the size string calculation
	// Extract this into a function with 2 params, size, po.kibi

	// Unnecessary lambda is unnecessary
	// MAYBE: change these to KiB if -k is provided, cause we have that here
	const std::string size_color = [&] {
		if(m_file_size < 1'000'000ULL)	  // 1MB
			return Color::rgb(255, 255, 255);
		else if(m_file_size < 128'000'000ULL)	 // 128MB
			return Color::rgb(113, 220, 208);
		else if(m_file_size < 512'000'000ULL)	 // 512MB
			return Color::rgb(250, 250, 48);
		else if(m_file_size < 1'000'000'000ULL)	   // 1GB
			return Color::rgb(223, 134, 89);
		return Color::rgb(0, 255, 255);
	}();

	// TODO: Add long listing format with user group info and time and perms
	// TODO: Symlink point stuff
	// FIXME: Change this 5 to the maximum length of the sizes
	return fmt::format("  {}  {}  {} {}{:>5}{}  {}  {}\n",
					   this->get_perms_as_string(),
					   "beronthecolossus",
					   "beronthecolossus",
					   size_color,
					   get_size_as_string(po.human, po.kibi),
					   Color::RESET,
					   get_modification_time(),
					   icon_and_color_filename());
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

// WOW This is fucking ugly!
std::string File::get_perms_as_string() const noexcept
{
	// MAYBE: make the else a function, or just pass in the bool and the permission?
	std::string result;
	result.reserve(225ULL);
	fs::perms perm = fs::symlink_status(m_file_path).permissions();
	if((perm & fs::perms::owner_read) != fs::perms::none)
	{
		result.append(Color::PERM_READ);
		result.push_back('r');
	}
	else
	{
		result.append(Color::PERM_NONE);
		result.push_back('-');
	}
	if((perm & fs::perms::owner_write) != fs::perms::none)
	{
		result.append(Color::PERM_WRITE);
		result.push_back('w');
	}
	else
	{
		result.append(Color::PERM_NONE);
		result.push_back('-');
	}
	if((perm & fs::perms::owner_exec) != fs::perms::none)
	{
		result.append(Color::PERM_EXEC);
		result.push_back('x');
	}
	else
	{
		result.append(Color::PERM_NONE);
		result.push_back('-');
	}
	if((perm & fs::perms::group_read) != fs::perms::none)
	{
		result.append(Color::PERM_READ);
		result.push_back('r');
	}
	else
	{
		result.append(Color::PERM_NONE);
		result.push_back('-');
	}
	if((perm & fs::perms::group_write) != fs::perms::none)
	{
		result.append(Color::PERM_WRITE);
		result.push_back('w');
	}
	else
	{
		result.append(Color::PERM_NONE);
		result.push_back('-');
	}
	if((perm & fs::perms::group_exec) != fs::perms::none)
	{
		result.append(Color::PERM_EXEC);
		result.push_back('x');
	}
	else
	{
		result.append(Color::PERM_NONE);
		result.push_back('-');
	}
	if((perm & fs::perms::others_read) != fs::perms::none)
	{
		result.append(Color::PERM_READ);
		result.push_back('r');
	}
	else
	{
		result.append(Color::PERM_NONE);
		result.push_back('-');
	}
	if((perm & fs::perms::others_write) != fs::perms::none)
	{
		result.append(Color::PERM_WRITE);
		result.push_back('w');
	}
	else
	{
		result.append(Color::PERM_NONE);
		result.push_back('-');
	}
	if((perm & fs::perms::others_exec) != fs::perms::none)
	{
		result.append(Color::PERM_EXEC);
		result.push_back('x');
	}
	else
	{
		result.append(Color::PERM_NONE);
		result.push_back('-');
	}
	result.append(Color::RESET);
	return result;
}

static constexpr std::array si_sizes {"B", "K", "M", "G", "T"};
static constexpr std::array kibi_sizes {"Bi", "Ki", "Mi", "Gi", "Ti"};

std::string File::get_size_as_string(const bool human, const bool kibi) const noexcept
{
	if(!human)
		return fmt::to_string(m_file_size);

	const size_t base = kibi ? 1024 : 1000;
	if(m_file_size < base)
		return fmt::format("{}{}", m_file_size, kibi ? kibi_sizes.front() : si_sizes.front());

	size_t size = m_file_size;
	size_t power_counter = 0ULL;
	while(size / 10ULL * 10ULL >= base)
	{
		size /= base;
		++power_counter;
	}

	return fmt::format("{}{}", size, kibi ? kibi_sizes[power_counter] : si_sizes[power_counter]);
}

std::string File::get_modification_time() const noexcept
{
	/* Waiting until GCC gets support for chrono formatting
	const auto modify_time = fs::last_write_time(m_file_path);
	const auto epoch_time = std::chrono::file_clock::to_sys(modify_time).time_since_epoch().count();
	const char* format_time = std::ctime(&epoch_time);
	return fmt::format("{}", format_time == nullptr ? "NULL" : format_time);
	*/

	// TODO: Find the difference of modify_time and now(), get a color according to that

	struct stat temp_stat;
	// Just do stat() for now, if dead links create an issue, switch to lstat
	// What about statx()
	stat(m_file_name.c_str(), &temp_stat);
	const auto modify_time = temp_stat.st_mtim.tv_sec;
	std::string result = std::ctime(&modify_time);
	result.erase(result.end() - 1ULL);
	return result;
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
		case fs::file_type::block:
		{
			m_color = Color::WHITE;
			m_indicator = "\u2588";
			return;
		}
		case fs::file_type::character:
		{
			m_color = Color::WHITE;
			m_indicator = "c";
			return;
		}
	}
}

}	 // namespace OK
