#pragma once

#include "Color.hpp"

#include <filesystem>
#include <fmt/core.h>
#include <string_view>

// TODO: Check if the terminal supports TrueColor, if not check for 8-bit then use them
// TODO: #include <unistd.h> isatty(STDOUT_FILENO), if so no colors

namespace fs = std::filesystem;

namespace OK
{
class File final
{
public:
	File(const std::string_view file_name);

	[[nodiscard]] std::string_view file_name() const noexcept { return m_file_name; }
	[[nodiscard]] std::string_view filename() const noexcept { return m_file_name; }
	[[nodiscard]] std::size_t size() const noexcept { return m_file_size; }
	[[nodiscard]] bool is_directory() const noexcept { return m_is_directory; }

private:
	const fs::path m_file_path;
	const std::string_view m_file_name;
	const bool m_is_directory;
	const bool m_is_symlink;
	const std::size_t m_file_size;
};

}	 // namespace OK
