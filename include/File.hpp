#pragma once

#include "Args.hpp"
#include "Color.hpp"
#include "Icons.hpp"

#include <filesystem>
#include <string_view>

// TODO: Check if the terminal supports TrueColor, if not check for 8-bit then use them
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

	[[nodiscard]] std::string to_string(const ParsedOptions) const noexcept;
	[[nodiscard]] std::size_t string_length(const bool long_listing) const noexcept;
	[[nodiscard]] std::string get_perms_as_string() const noexcept;
	[[nodiscard]] std::string get_size_as_string(const bool human, const bool kibi = false) const
		noexcept;
	[[nodiscard]] std::string get_modification_time() const noexcept;

	// The order is Directories first, then Files, and dotfiles are first in both categories,
	// lowercase comparison, find a way to get lowercase unicode characters
	[[nodiscard]] bool operator<(const File& other) const;

	[[nodiscard]] bool operator==(const File& other) const
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

	void handle_icon_and_color() noexcept;
	[[nodiscard]] std::string icon_and_color_filename() const noexcept;
};

}	 // namespace OK
