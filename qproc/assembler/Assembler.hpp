#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

class Assembler
{
public:
	Assembler(std::string source_path, std::string dest_path);
	
	void assemble();
	
	void error(std::string msg);
	
private:
	std::ifstream input;
	std::ofstream output;
	
	size_t current_memory_location;
};

#endif // ASSEMBLER_H
