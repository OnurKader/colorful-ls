#include "File.hpp"

namespace OK
{
File::File(const std::string_view file_name) :
	m_file_path {file_name},
	m_file_name {file_name},
	m_is_directory {fs::is_directory(m_file_path)},
	m_file_size {m_is_directory ? 4096ULL : fs::file_size(m_file_path)}
{
}

}	 // namespace OK
