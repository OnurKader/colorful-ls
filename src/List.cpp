#include "Args.hpp"
#include "File.hpp"
#include "FileVec.hpp"
#include "Utils.hpp"

#include <clocale>
#include <filesystem>
#include <fmt/format.h>
#include <vector>

int main(int argc, char** argv)
{
	Timer timer {"The whole shebang"};
	std::setlocale(LC_ALL, "");
	OK::Args args;
	const auto results_opt = args.parse(argc, argv);
	if(!results_opt)
		return 69;
	const auto results = results_opt.value();

	// FIXME: put positional arguments in a vector and do printings for those, if it's a directory
	// put the name of it above the listings

	// MAYBE: Do CTRE regex stuff as an argument?
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

		if(num_of_files_in_directory == 0L)
		{
			fmt::print(stderr,
					   FMT_STRING("    {}Nothing to show here...\n"),
					   OK::Color::rgb(229, 195, 38));
			return 0;
		}

		// The actual printing
		OK::FileVec file_vec {std::move(input_file), num_of_files_in_directory, results.all};

		file_vec.print(results);
	}
	else
	{
		fmt::print("{}",
				   input_file.long_name_to_string(results,
												  input_file.get_size_digit_count(),
												  input_file.username().size(),
												  input_file.groupname().size()));
	}

	return 0;
}
