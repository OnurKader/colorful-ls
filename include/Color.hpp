#pragma once

#include <fmt/format.h>

namespace OK
{
namespace Color
{
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
static constexpr std::string_view SOCK {"\033[38;2;130;20;175m"};
static constexpr std::string_view PIPE {"\033[38;2;151;155;160m"};
static constexpr std::string_view REG_LINK {"\033[38;2;232;254;32m"};
static constexpr std::string_view DIR_LINK {"\033[38;2;100;254;164m"};
static constexpr std::string_view BRKN_LINK {"\033[38;2;222;75;18m"};

static constexpr std::string_view m_rgb_template {"\033[38;2;{};{};{}m"};

inline std::string rgb(const uint8_t r, const uint8_t g, const uint8_t b)
{
	return fmt::format(m_rgb_template, r, g, b);
}
}	 // namespace Color

}	 // namespace OK
