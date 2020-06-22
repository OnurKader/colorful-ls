#include "Args.hpp"
#include "File.hpp"

// TODO: Look into fmt::os.h

#include <filesystem>
#include <fmt/core.h>
#include <vector>

namespace fs = std::filesystem;

// sgetRGB(buff, 229, 195, 38); // Empty directory colors

int main(int argc, char** argv)
{
	OK::Args args;
	const auto results_opt = args.parse(argc, argv);
	if(!results_opt)
		return 1;
	const auto results = results_opt.value();

	OK::File file {argc == 1 ? "." : argv[1]};
	if(fs::is_directory(file.path()))
	{
		// Directory, go into it, don't recurse
		for(auto&& path: fs::directory_iterator(file.path()))
			fmt::print(u8"\t{}: {}\n",
					   OK::File {path}.to_string(results.long_listing),
					   OK::File {path}.string_length(false));
	}
	else
	{
		// Non-directory
		fmt::print(u8"\t{}\n", file.to_string(results.long_listing));
	}

	return 0;
}
