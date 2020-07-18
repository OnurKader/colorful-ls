#include "FileVec.hpp"

#include <numeric>
#include <sys/ioctl.h>
#include <unistd.h>

namespace OK
{
void FileVec::print_long(ParsedOptions options) const
{
	for(const auto& file: m_files)
	{
		fmt::print(FMT_STRING("{}"), file.long_name_to_string(options));
	}
}

uint16_t get_term_width() noexcept
{
	struct winsize ws;
	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != 0)
		return 80U;
	return ws.ws_col;
}

uint64_t get_total_file_name_length(const FileVec& fv)
{
	return std::accumulate(fv.cbegin(), fv.cend(), 0ULL, [](auto&& lhs, const auto& rhs) {
		return lhs + rhs.string_length();
	});
}

#ifndef self
#define self *this
#endif

void FileVec::print_columnal() const
{
	const auto term_width = get_term_width();
	std::size_t total_file_name_length = get_total_file_name_length(self);
	// MAYBE: Custom fmt formatter for file? If so how? maybe store the width in FileVec and write
	// the formatter for that
}

void FileVec::print(ParsedOptions options) const
{
	if(options.long_listing)
		print_long(options);
	else
		print_columnal();
}

}	 // namespace OK
