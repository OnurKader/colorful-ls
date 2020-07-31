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

	// MAYBE: Do CTRE regex stuff as an argument?
	// man glob
	if(argc == 1)
	{
		OK::File input_file {"."};
		if(!fs::exists(input_file.path()))
		{
			fmt::print(
				stderr, "    {}File or directory not found{}\n", OK::Color::RED, OK::Color::RESET);
			return 1;
		}

		const auto num_of_files_in_directory =
			std::distance(std::filesystem::directory_iterator {input_file.path()},
						  std::filesystem::directory_iterator {});

		if(num_of_files_in_directory == 0L)
		{
			fmt::print(stderr,
					   "    {}Nothing to show here...{}\n",
					   OK::Color::rgb(229, 195, 38),
					   OK::Color::RESET);
			return 0;
		}

		// The actual printing
		OK::FileVec file_vec {std::move(input_file), num_of_files_in_directory, results};
		file_vec.print();
		return file_vec.return_value();
	}
	else
	{
		// Multiple inputs in argv, put them in a vector and print those
		OK::FileVec file_vec {argc, argv, results};
		file_vec.print();

		return file_vec.return_value();
	}

	return 0;
}
