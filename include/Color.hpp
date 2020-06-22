#pragma once

#include <fmt/core.h>

namespace OK
{
class Color final
{
public:
	static constexpr std::u8string_view RESET {u8"\033[m"};
	static constexpr std::u8string_view WHITE {u8"\033[38;2;255;255;255m"};
	static constexpr std::u8string_view BLACK {u8"\033[38;2;0;0;0m"};
	static constexpr std::u8string_view RED {u8"\033[38;2;255;0;0m"};
	static constexpr std::u8string_view GREEN {u8"\033[38;2;0;255;0m"};
	static constexpr std::u8string_view BLUE {u8"\033[38;2;0;0;255m"};
	static constexpr std::u8string_view CYAN {u8"\033[38;2;0;255;255m"};
	static constexpr std::u8string_view MAGENTA {u8"\033[38;2;255;0;255m"};
	static constexpr std::u8string_view YELLOW {u8"\033[38;2;255;255;0m"};
	static constexpr std::u8string_view DIR {u8"\033[38;2;20;162;255m"};
	static constexpr std::u8string_view FILE {u8"\033[38;2;1;255;1m"};
	static constexpr std::u8string_view SOCK {u8"\033[38;2;130;20;175m"};
	static constexpr std::u8string_view PIPE {u8"\033[38;2;151;155;160m"};
	static constexpr std::u8string_view REG_LINK {u8"\033[38;2;232;254;32m"};
	static constexpr std::u8string_view DIR_LINK {u8"\033[38;2;100;254;164m"};
	static constexpr std::u8string_view BRKN_LINK {u8"\033[38;2;222;75;18m"};

	static std::u8string rgb(const uint8_t r, const uint8_t g, const uint8_t b)
	{
		return fmt::format(m_rgb_template, r, g, b);
	}

private:
	static constexpr std::u8string_view m_rgb_template {u8"\033[38;2;{};{};{}m"};
};

}	 // namespace OK
