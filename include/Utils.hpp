#pragma once

#include <chrono>
#include <fmt/core.h>
#include <string_view>
// #include <source_control>?

#ifndef massert
#define _TO_STR(x) #x
#define TO_STR(x)  #x
#define massert(cond, message)                                 \
	if(!(cond))                                                \
	{                                                          \
		fmt::print(stderr,                                     \
				   "{}:{}: {}: Assertion `{}` failed. ({})\n", \
				   __FILE__,                                   \
				   __LINE__,                                   \
				   __FUNCTION__,                               \
				   TO_STR(cond),                               \
				   message);                                   \
		std::abort();                                          \
	}
#endif

using hr_clock = std::chrono::high_resolution_clock;

struct Timer final
{
	explicit Timer(std::string_view name) : m_time {hr_clock::now()}, m_name {name} {}
	~Timer()
	{
		const int64_t duration_as_T =
			std::chrono::duration_cast<std::chrono::microseconds>(hr_clock::now() - m_time).count();
		fmt::print(stderr, "{} took {} µs, {} ms\n", m_name, duration_as_T, duration_as_T / 1000L);
	}

	const hr_clock::time_point m_time;
	const std::string_view m_name;
};
