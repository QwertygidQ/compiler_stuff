#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

class Assembler
{
public:
	Assembler(std::string source_path, std::string dest_path);
	~Assembler();
	
	void assemble();
	
private:
	void swap_endianness(int32_t* value);
	void error(std::string msg);
	void write_program();
	
	std::ifstream input;
	std::ofstream output;
	
	std::vector<uint8_t> program;
};

#endif // ASSEMBLER_H
