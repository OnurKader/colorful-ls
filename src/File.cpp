#include "File.hpp"

#include <algorithm>
#include <cwchar>
#include <fmt/chrono.h>
#include <fmt/format.h>

// Sorry for this!
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>
#include <unistd.h>

namespace OK
{
inline std::string mb_lower(const std::string& mb_str);

File::File(const fs::path file_path) :
	m_file_path {file_path},
	m_file_type {fs::symlink_status(m_file_path).type()},
	m_file_name {file_path.string()},
	m_time_color {Color::rgb(20, 255, 42)}
{
	if(fs::is_directory(m_file_path))
	{
		m_file_size = 4096ULL;
	}
	else if(fs::exists(m_file_path))
	{
		std::error_code ec;
		const auto file_size_or_error = fs::file_size(m_file_path, ec);
		if(file_size_or_error != static_cast<std::uintmax_t>(-1LL))
			m_file_size = file_size_or_error;
		else
			m_file_size = 0ULL;
	}
	else
	{
		m_file_size = 0ULL;
	}

	m_file_name.erase(0ULL, m_file_name.find_last_of('/') + 1ULL);

	m_extension = get_ext_from_filename(m_file_name);
	handle_icon_and_color();

	// ???: Is this really necessary? Could we just do a naive ASCII to_lower function?
	// TODO: In the mb_* functions use fmt::detail::utf8_to_utf16 or whatever it's called
	m_mb_lowercase_name = mb_lower(m_file_name);

	// Stat the h*ck out of this file
	lstat(m_file_path.c_str(), &m_lstat);

	handle_user_and_groupname();
	handle_modify_time_and_color();
}

File::File(File&& other) = default;

File& File::operator=(File&& other) = default;

// fmt FTW
inline auto mb_to_wstring(const std::string& mb_str)
{
	std::array<wchar_t, 128ULL> temp_wide {L'\0'};
	// mbtowcs returns length of wstring, should we erase the last parts? Nah.
	std::mbstowcs(temp_wide.data(), mb_str.data(), mb_str.size());
	return temp_wide;
}

inline void ws_tolower(std::array<wchar_t, 128ULL>& wstr)
{
	std::transform(wstr.begin(), wstr.end(), wstr.begin(), [loc = std::locale {""}](const auto& c) {
		return std::tolower(c, loc);
	});
}

inline std::string w_to_mbstring(const std::array<wchar_t, 128ULL>& wstr)
{
	std::string temp_mb(wstr.size(), '\0');
	std::wcstombs(temp_mb.data(), wstr.data(), wstr.size());
	return temp_mb;
}

inline std::string mb_lower(const std::string& mb_str)
{
	auto temp = mb_to_wstring(mb_str);
	ws_tolower(temp);
	return w_to_mbstring(temp);
}

bool File::operator<(const File& other) const noexcept
{
	using ft = fs::file_type;
	ft this_type = m_file_type;
	ft other_type = other.m_file_type;

	if(this_type == ft::symlink)
	{
		const auto followed_stat = fs::status(m_file_path);
		if(fs::exists(followed_stat))
			this_type = followed_stat.type();
	}

	if(other_type == ft::symlink)
	{
		const auto followed_stat = fs::status(other.m_file_path);
		if(fs::exists(followed_stat))
			other_type = followed_stat.type();
	}

	if(this_type == ft::directory && other_type != ft::directory)
	{
		return true;
	}
	else if(this_type != ft::directory && other_type == ft::directory)
	{
		return false;
	}
	else
	{
		const auto& lhs = m_mb_lowercase_name;
		const auto& rhs = other.m_mb_lowercase_name;
		const auto retval = strverscmp(lhs.c_str(), rhs.c_str());

		return retval < 0;
	}
}

std::string File::icon_and_color_filename() const noexcept
{
	return fmt::format(
		FMT_STRING("{}{}{}{}{}"), m_color, m_icon, m_file_name, Color::RESET, m_indicator);
}

uint64_t File::icon_and_color_filename_length() const noexcept
{
	return (m_color.size() + 1ULL + m_file_name.size() + Color::RESET.size() + m_indicator.size());
}

std::string File::long_name_to_string(ParsedOptions po,
									  std::size_t size_digit_count,
									  std::size_t longest_username_length,
									  std::size_t longest_groupname_length) const noexcept
{
	// MAYBE: Extract this into a function with 2 params, size, po.kibi

	// Unnecessary lambda is unnecessary
	// MAYBE: change these to KiB if -k is provided, cause we have that here
	// TODO: Move this to where the size is calculated, or just in the constructor
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

	const auto modification_time = get_modification_time();

	// TODO: Symlink point stuff
	return fmt::format(FMT_STRING("  {}  {:>{}}  {}{:>{}}{} {}{:>{}}{}  {}{}{}  {}\n"),
					   get_perms_as_string(),
					   m_username,
					   longest_username_length,
					   Color::rgb(207, 198, 104),
					   m_groupname,
					   longest_groupname_length,
					   Color::RESET,
					   size_color,
					   get_size_as_string(po.human, po.kibi),
					   size_digit_count,
					   Color::RESET,
					   m_time_color,
					   modification_time,
					   Color::RESET,
					   icon_and_color_filename());
}

inline std::size_t mb_strlen(std::string_view str) { return fmt::detail::count_code_points(str); }

uint64_t File::string_length() const noexcept
{
	std::size_t total_length = mb_strlen(m_icon);
	total_length += mb_strlen(m_file_name);
	total_length += m_indicator.size();
	total_length += mb_strlen(m_indicator);
	return total_length;
}

std::string File::get_perms_as_string() const noexcept
{
	std::string result;
	result.reserve(206UL);
	fs::perms perm = fs::symlink_status(m_file_path).permissions();

	const auto no_perm_l = [&result] {
		result.append(Color::PERM_NONE);
		result.push_back('-');
	};

	const auto handle_specific_perm_l = [&result](fs::perms permission) {
		switch(permission)
		{
			case fs::perms::owner_read: [[fallthrough]];
			case fs::perms::group_read: [[fallthrough]];
			case fs::perms::others_read:
			{
				result.append(Color::PERM_READ);
				result.push_back('r');
				break;
			}

			case fs::perms::owner_write: [[fallthrough]];
			case fs::perms::group_write: [[fallthrough]];
			case fs::perms::others_write:
			{
				result.append(Color::PERM_WRITE);
				result.push_back('w');
				break;
			}

			case fs::perms::owner_exec: [[fallthrough]];
			case fs::perms::group_exec: [[fallthrough]];
			case fs::perms::others_exec:
			{
				result.append(Color::PERM_EXEC);
				result.push_back('x');
				break;
			}
			default: fmt::print("What are you doing here with that permission boy!\n");
		}
	};

#define PERM_STRING(p)                \
	if((perm & p) != fs::perms::none) \
	{                                 \
		handle_specific_perm_l(p);    \
	}                                 \
	else                              \
	{                                 \
		no_perm_l();                  \
	}

	PERM_STRING(fs::perms::owner_read);
	PERM_STRING(fs::perms::owner_write);
	PERM_STRING(fs::perms::owner_exec);
	PERM_STRING(fs::perms::group_read);
	PERM_STRING(fs::perms::group_write);
	PERM_STRING(fs::perms::group_exec);
	PERM_STRING(fs::perms::others_read);
	PERM_STRING(fs::perms::others_write);
	PERM_STRING(fs::perms::others_exec);

#undef PERM_STRING

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
		return fmt::format(
			FMT_STRING("{}{}"), m_file_size, kibi ? kibi_sizes.front() : si_sizes.front());

	size_t size = m_file_size;
	size_t power_counter = 0ULL;
	while(size / 10ULL * 10ULL >= base)
	{
		size /= base;
		++power_counter;
	}

	return fmt::format("{}{}", size, kibi ? kibi_sizes[power_counter] : si_sizes[power_counter]);
}

static constexpr std::size_t number_of_digits(std::size_t num) noexcept
{
	std::size_t digit_count = 1ULL;
	while(num /= 10)
		++digit_count;

	return digit_count;
}

std::size_t File::get_size_digit_count() const noexcept { return number_of_digits(m_file_size); }

std::string File::get_modification_time() const noexcept
{
	/* Waiting until GCC gets support for chrono formatting
	const auto modify_time = fs::last_write_time(m_file_path);
	const auto epoch_time = std::chrono::file_clock::to_sys(modify_time).time_since_epoch().count();
	const char* format_time = std::ctime(&epoch_time);
	return fmt::format("{}", format_time == nullptr ? "NULL" : format_time);
	*/

	std::string result = std::ctime(&m_modify_time);
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
			if((fs::status(m_file_path).permissions() & fs::perms::owner_exec) != fs::perms::none)
				m_indicator = "*";
			else
				m_indicator = " ";
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

void File::handle_user_and_groupname()
{
	const struct passwd* pw = getpwuid(m_lstat.st_uid);
	const struct group* gr = getgrgid(m_lstat.st_gid);

	m_username = pw->pw_name;
	m_groupname = gr->gr_name;
}

void File::handle_modify_time_and_color()
{
	m_modify_time = m_lstat.st_mtim.tv_sec;

	// Color determination
	static constexpr long long HOUR = 60LL * 60LL;	  // An hour in seconds
	static constexpr long long DAY = 24LL * HOUR;	  // A day in seconds
	// Why not just use chrono???
	const auto now = std::time(nullptr);
	const auto time_diff = now - m_modify_time;
	if(time_diff > 3LL * DAY)
		m_time_color = Color::rgb(32, 123, 121);
	else if(time_diff > DAY)
		m_time_color = Color::rgb(73, 144, 241);
	else if(time_diff > 6 * HOUR)
		m_time_color = Color::rgb(108, 222, 172);
	else if(time_diff > HOUR)
		m_time_color = Color::rgb(148, 242, 192);
}

}	 // namespace OK
