project('List', 'cpp',
	version : '0.1',
	license : 'MIT',
	default_options : ['cpp_std=c++2a', 'buildtype=debugoptimized', 'warning_level=3'])

sources = ['src/File.cpp', 'src/FileVec.cpp', 'src/List.cpp']

cpp_args = ['-flto', '-Wformat-overflow=2', '-Wpedantic', '-Wshadow', '-Wmissing-include-dirs', '-Wuninitialized', '-Wduplicated-branches', '-Wduplicated-cond', '-Wfloat-equal', '-Wconversion', '-Wcast-qual', '-Wmissing-declarations', '-fno-rtti']

includes = include_directories('include')

dep = dependency('fmt')

exe = executable('List-meson', sources : sources, dependencies : dep, include_directories : includes, cpp_args: cpp_args)

test('Simple Test', exe)

