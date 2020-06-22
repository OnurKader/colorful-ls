#include "Args.hpp"
#include "File.hpp"

#include <clocale>
#include <filesystem>
#include <fmt/format.h>
#include <vector>

namespace fs = std::filesystem;

// sgetRGB(buff, 229, 195, 38); // Empty directory colors

int main(int argc, char** argv)
{
	std::setlocale(LC_ALL, "en_GB.utf8");
	OK::Args args;
	const auto results_opt = args.parse(argc, argv);
	if(!results_opt)
		return 1;
	const auto results = results_opt.value();

	OK::File file {argc == 1 ? "." : argv[1]};
	if(fs::is_directory(file.path()))
	{
		for(auto&& path: fs::directory_iterator(file.path()))
			fmt::print("\t{:<45} - {}\n",
					   OK::File {path}.to_string(results.long_listing),
					   OK::File {path}.string_length(results.long_listing));
	}
	else
	{
		fmt::print("\t{}\n", file.to_string(results.long_listing));
	}

	return 0;
}
