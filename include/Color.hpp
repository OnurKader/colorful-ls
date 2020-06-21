#pragma once

#include <fmt/core.h>

// FIXME: Use wstrings

namespace OK
{
class Color final
{
public:
	static constexpr std::string_view RESET {"\033[m"};
	static constexpr std::string_view WHITE {"\033[38;2;255;255;255m"};
	static constexpr std::string_view BLACK {"\033[38;2;0;0;0m"};
	static constexpr std::string_view RED {"\033[38;2;255;0;0m"};
	static constexpr std::string_view GREEN {"\033[38;2;0;255;0m"};
	static constexpr std::string_view BLUE {"\033[38;2;0;0;255m"};
	static constexpr std::string_view CYAN {"\033[38;2;0;255;255m"};
	static constexpr std::string_view MAGENTA {"\033[38;2;255;0;255m"};
	static constexpr std::string_view YELLOW {"\033[38;2;255;255;0m"};
	static constexpr std::string_view DIR {"\033[38;2;20;162;255m"};
	static constexpr std::string_view FILE {"\033[38;2;1;255;1m"};

	static std::string rgb(const uint8_t r, const uint8_t g, const uint8_t b)
	{
		return fmt::format(m_rgb_template, r, g, b);
	}

	static std::string eight(const uint8_t c) { return fmt::format(m_8_template, c); }
	static std::string eight_bright(const uint8_t c) { return fmt::format(m_8_bright_template, c); }

private:
	static constexpr std::string_view m_rgb_template {"\033[38;2;{};{};{}m"};
	static constexpr std::string_view m_8_template {"\033[{}m"};
	static constexpr std::string_view m_8_bright_template {"\033[1;{}m"};
};

}	 // namespace OK
