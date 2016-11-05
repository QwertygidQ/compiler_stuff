#include "Assembler.hpp"

#include <iostream>

int main()
{
	std::cout << "Assembler for QProc" << std::endl <<
		"Qwertygid, 2016" << std::endl << std::endl;
	
	std::cout << "Enter source code's location: ";
	std::string source_path;
	std::cin >> source_path;
	
	std::cout << "Enter resulting ROM's location: ";
	std::string dest_path;
	std::cin >> dest_path;
	
	Assembler assembler(source_path, dest_path);
	assembler.assemble();
	
	return EXIT_SUCCESS;
}