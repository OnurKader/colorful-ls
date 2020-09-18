#include "Args.hpp"
#include "File.hpp"
#include "FileVec.hpp"
#include "Utils.hpp"

// FIXME: List -a /usr/local/bin throws a SEGFAULT, look at file type handling and permissions

int main(int argc, char** argv)
{
	const Timer timer {"The whole shebang"};

	auto args = OK::Args {};
	const auto results_opt = args.parse(argc, argv);
	if(!results_opt)
		return 1;
	const auto results = results_opt.value();
	const auto& filenames = args.positionals();

	const auto number_of_filenames = filenames.size();

	if(number_of_filenames < 2UL)
	{
		OK::File input_file {number_of_filenames == 0UL ? "." : filenames.front()};
		if(!fs::exists(input_file.path()))
		{
			fmt::print(
				stderr, "    {}File or directory not found{}\n", OK::Color::RED, OK::Color::RESET);
			return 1;
		}

		// FIXME: Check if input_file is a directory or not
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

		// The actual printing of the directory/file
		const OK::FileVec file_vec {std::move(input_file), num_of_files_in_directory, results};
		file_vec.print();

		return file_vec.return_value();
	}
	else
	{
		// Construct a FileVec from the vector of strings we got in the args
		const OK::FileVec file_vec {filenames, results};
		file_vec.print();

		return file_vec.return_value();
	}

	return 0;
}
