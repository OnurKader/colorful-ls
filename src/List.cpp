#include "Args.hpp"
#include "File.hpp"

// TODO: Look into fmt::os.h

#include <filesystem>
#include <fmt/core.h>

namespace fs = std::filesystem;

int main(int argc, char** argv)
{
	OK::Args args;
	const auto results = args.parse(argc, argv);
	if(!results)
		return 1;

	const std::string_view queried_path {argc == 1 ? "." : argv[1]};
	OK::File queried_file(queried_path);

	fmt::print("File stuff: {}-{}B\n", queried_file.filename(), queried_file.size());

	return 0;
}
