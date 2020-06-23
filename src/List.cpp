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
		return 69;
	const auto results = results_opt.value();

	OK::File file {argc == 1 ? "." : argv[1]};
	if(!fs::exists(file.path()))
	{
		fmt::print(
			stderr, "    {}File or directory not found{}\n", OK::Color::RED, OK::Color::RESET);
		return 1;
	}
	else if(fs::is_directory(file.path()))
	{
		const auto num_of_files_in_directory =
			std::distance(std::filesystem::directory_iterator {file.path()},
						  std::filesystem::directory_iterator {});
		if(num_of_files_in_directory == 0U)
		{
			fmt::print(stderr, "    {}Nothing to show here...\n", OK::Color::rgb(229, 195, 38));
			return 0;
		}

		// The actual printing

		for(auto&& path: fs::directory_iterator(file.path()))
			fmt::print("{}", OK::File {path}.to_string(results));
	}
	else
	{
		fmt::print("{}", file.to_string(results));
	}

	return 0;
}
