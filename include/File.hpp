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
class File final
{
public:
	explicit File(const fs::path file_path);

	[[nodiscard]] std::u8string_view name() const noexcept { return m_file_name; }
	[[nodiscard]] fs::path path() const noexcept { return m_file_path; }
	[[nodiscard]] std::size_t size() const noexcept { return m_file_size; }
	[[nodiscard]] fs::file_type type() const noexcept { return m_file_type; }

	[[nodiscard]] std::u8string to_string(const bool long_listing) const noexcept;
	[[nodiscard]] std::size_t string_length(const bool long_listing) const noexcept;

private:
	const fs::path m_file_path;
	const fs::file_type m_file_type;

	std::u8string m_file_name;
	std::size_t m_file_size;
	std::u8string_view m_extension;
	std::u8string_view m_icon;
	std::u8string_view m_color;
	std::u8string_view m_indicator;

	void handle_icon_and_color() noexcept;
};

}	 // namespace OK
