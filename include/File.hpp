#pragma once

#include "Color.hpp"
#include "Icons.hpp"

#include <filesystem>
#include <string_view>

// TODO: Check if the terminal supports TrueColor, if not check for 8-bit then use them
// TODO: #include <unistd.h> isatty(STDOUT_FILENO), if so no colors

namespace fs = std::filesystem;

namespace OK
{
// MAYBE: Have a Type enum with like REG, DIR, SOCK, PIPE, LINK stuff like that and return that upon
// request?

class File final
{
public:
	File(const std::string_view file_name);

	[[nodiscard]] std::string_view filename() const noexcept { return m_file_name; }
	[[nodiscard]] std::string_view name() const noexcept { return m_file_name; }
	[[nodiscard]] fs::path filepath() const noexcept { return m_file_path; }
	[[nodiscard]] fs::path path() const noexcept { return m_file_path; }
	[[nodiscard]] std::size_t size() const noexcept { return m_file_size; }
	// Instead of this have a type one?
	[[nodiscard]] bool is_directory() const noexcept { return m_is_directory; }

private:
	const fs::path m_file_path;
	const std::string_view m_file_name;
	const bool m_is_directory;
	const std::size_t m_file_size;

	std::string_view m_icon;
	std::string_view m_color;
	std::string_view m_extension;
};

}	 // namespace OK
