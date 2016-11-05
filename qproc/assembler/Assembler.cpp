#include "Assembler.hpp"

#include <iostream>

Assembler::Assembler(std::string source_path, std::string dest_path)
{
	input.open(source_path);
	if (!input.is_open())
		error("Failed to open source code");
}

void Assembler::assemble()
{
}

void Assembler::error(std::string msg)
{
	std::cerr << msg << std::endl;
	exit(EXIT_FAILURE);
}