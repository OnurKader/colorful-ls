#pragma once

#include "Args.hpp"
#include "Color.hpp"
#include "Icons.hpp"

#include <filesystem>
#include <string_view>
#include <sys/stat.h>

// TODO: Check if the terminal supports TrueColor, if not check for 8-bit then use them
// ^ It's just $COLORTERM on modern terminals (COLORTERM == truecolor)

// TODO: #include <unistd.h> isatty(STDOUT_FILENO), if so no colors

namespace fs = std::filesystem;

namespace OK
{
class File final
{
public:
	explicit File(const fs::path file_path);
	File(File&&);

	File& operator=(File&&);

	[[nodiscard]] std::string_view name() const noexcept { return m_file_name; }
	[[nodiscard]] fs::path path() const noexcept { return m_file_path; }
	[[nodiscard]] std::size_t size() const noexcept { return m_file_size; }
	[[nodiscard]] fs::file_type type() const noexcept { return m_file_type; }
	[[nodiscard]] const std::string& username() const noexcept { return m_username; }
	[[nodiscard]] const std::string& groupname() const noexcept { return m_groupname; }

	// Really 4 arguments?
	[[nodiscard]] std::string long_name_to_string(
		ParsedOptions,
		std::size_t size_digit_count,
		std::size_t longest_username_length,
		std::size_t longest_groupname_length) const noexcept;
	[[nodiscard]] std::string icon_and_color_filename() const noexcept;
	[[nodiscard]] uint64_t icon_and_color_filename_length() const noexcept;
	[[nodiscard]] uint64_t string_length() const noexcept;
	[[nodiscard]] std::string get_perms_as_string() const noexcept;
	[[nodiscard]] std::string get_size_as_string(const bool human,
												 const bool kibi = false) const noexcept;
	[[nodiscard]] std::size_t get_size_digit_count() const noexcept;
	[[nodiscard]] std::string get_modification_time() const noexcept;

	// The order is Directories first, then Files, and dotfiles are first in both categories,
	// lowercase comparison, find a way to get lowercase unicode characters
	[[nodiscard]] bool operator<(const File& other) const noexcept;

	[[nodiscard]] bool operator==(const File& other) const noexcept
	{
		return m_file_name == other.m_file_name;
	}

private:
	fs::path m_file_path;
	fs::file_type m_file_type;

	std::string m_file_name;
	std::size_t m_file_size;
	std::string_view m_extension;
	std::string_view m_icon;
	std::string_view m_color;
	std::string_view m_indicator;
	std::string m_username;
	std::string m_groupname;

	std::time_t m_modify_time;
	mutable std::string m_time_color;

	std::string m_mb_lowercase_name;
	struct stat m_lstat;

	void handle_icon_and_color() noexcept;
	void handle_user_and_groupname();
	void handle_modify_time_and_color();
};

}	 // namespace OK
