#include "Args.hpp"
#include "File.hpp"

#include <clocale>
#include <filesystem>
#include <fmt/format.h>
#include <vector>

// TODO: Add a directory_entry constructor for file or make a FileVector class that holds a vec and
// does the printing and shit

namespace fs = std::filesystem;

int main(int argc, char** argv)
{
	std::setlocale(LC_ALL, "");
	OK::Args args;
	const auto results_opt = args.parse(argc, argv);
	if(!results_opt)
		return 69;
	const auto results = results_opt.value();

	OK::File input_file {argc == 1 ? "." : argv[1]};
	if(!fs::exists(input_file.path()))
	{
		fmt::print(stderr,
				   FMT_STRING("    {}File or directory not found{}\n"),
				   OK::Color::RED,
				   OK::Color::RESET);
		return 1;
	}
	else if(fs::is_directory(input_file.path()))
	{
		const auto num_of_files_in_directory =
			std::distance(std::filesystem::directory_iterator {input_file.path()},
						  std::filesystem::directory_iterator {});

		if(num_of_files_in_directory == 0U)
		{
			fmt::print(stderr,
					   FMT_STRING("    {}Nothing to show here...\n"),
					   OK::Color::rgb(229, 195, 38));
			return 0;
		}

		std::vector<OK::File> files;
		files.reserve(static_cast<std::size_t>(num_of_files_in_directory));

		// The actual printing

		/*
		std::vector<OK::File> asd(
			(fs::directory_iterator(input_file.path()), fs::directory_iterator()));
		*/
		for(const auto& path: fs::directory_iterator(input_file.path()))
			files.emplace_back(path);

		std::sort(files.begin(), files.end());

		for(const auto& file: files)
			fmt::print("{}", file.to_string(results));
	}
	else
	{
		fmt::print("{}", input_file.to_string(results));
	}

	return 0;
}
