#pragma once

#include "Utils.hpp"

#include <array>
#include <cstring>

template<std::size_t S>
class CharArray final
{
public:
	CharArray() = default;

	CharArray(const char* str)
	{
		massert(std::strlen(str) <= S, "Passed const char* is larger than capacity");
		std::strcpy(m_data.data(), str);
	}

	template<std::size_t N>
	CharArray(char (&arr)[N])
	{
		massert(N <= S, "Passed a char[] larger than capacity");
		std::strcpy(m_data.data(), arr);
	}

	CharArray<S>& operator=(const char* str)
	{
		massert(std::strlen(str) <= S, "Passed const char* is larger than capacity");
		std::strcpy(m_data.data(), str);

		return *this;
	}

	template<std::size_t N>
	CharArray<S>& operator=(char (&arr)[N])
	{
		massert(N <= S, "Passed a char[] larger than capacity");
		std::strcpy(m_data.data(), arr);

		return *this;
	}

	auto data() { return m_data.data(); }
	auto data() const { return m_data.data(); }

	auto size() const { return S; }

	void clear() { std::fill(m_data.begin(), m_data.end(), '\0'); }

private:
	std::array<char, S> m_data;
};
