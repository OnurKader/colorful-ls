#pragma once

#include <fmt/core.h>

namespace OK
{
namespace Color
{
static constexpr std::string_view RESET {"\033[m"};
static constexpr std::string_view WHITE {"\033[38;2;255;255;255m"};
static constexpr std::string_view BLACK {"\033[38;2;000;000;000m"};
static constexpr std::string_view RED {"\033[38;2;255;000;000m"};
static constexpr std::string_view GREEN {"\033[38;2;000;255;000m"};
static constexpr std::string_view BLUE {"\033[38;2;000;000;255m"};
static constexpr std::string_view CYAN {"\033[38;2;000;255;255m"};
static constexpr std::string_view MAGENTA {"\033[38;2;255;000;255m"};
static constexpr std::string_view YELLOW {"\033[38;2;255;255;000m"};

static constexpr std::string_view DIR {"\033[38;2;020;162;255m"};
static constexpr std::string_view FILE {"\033[38;2;001;255;001m"};
static constexpr std::string_view SOCK {"\033[38;2;130;020;175m"};
static constexpr std::string_view PIPE {"\033[38;2;151;155;160m"};
static constexpr std::string_view REG_LINK {"\033[38;2;232;254;032m"};
static constexpr std::string_view DIR_LINK {"\033[38;2;100;254;164m"};
static constexpr std::string_view BRKN_LINK {"\033[38;2;222;075;018m"};

static constexpr std::string_view PERM_READ {"\033[38;2;230;220;059m"};
static constexpr std::string_view PERM_WRITE {"\033[38;2;042;228;052m"};
static constexpr std::string_view PERM_EXEC {"\033[38;2;224;058;032m"};
static constexpr std::string_view PERM_NONE {"\033[38;2;088;040;128m"};

static constexpr std::string_view m_rgb_template {"\033[38;2;{:03};{:03};{:03}m"};

inline std::string rgb(uint8_t r, uint8_t g, uint8_t b) noexcept
{
	return fmt::format(m_rgb_template, r, g, b);
}
}	 // namespace Color

}	 // namespace OK
