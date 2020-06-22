#include "Args.hpp"
#include "File.hpp"

// TODO: Look into fmt::os.h

#include <filesystem>
#include <fmt/core.h>
#include <vector>

namespace fs = std::filesystem;

int main(int argc, char** argv)
{
	OK::Args args;
	const auto results = args.parse(argc, argv);
	if(!results)
		return 1;

	std::vector<OK::File> files(&argv[1], &argv[argc]);

	for(auto&& file: files)
		fmt::print("Ext. {}\nFile stuff: {}-{}B\n",
				   get_ext_from_filename(file.name()),
				   file.name(),
				   file.size());

	return 0;
}
