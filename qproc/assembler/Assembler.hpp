#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include <cstdint>
#include <fstream>
#include <map>
#include <string>
#include <utility>
#include <vector>

class Assembler
{
public:
	Assembler(std::string source_path, std::string dest_path);
	~Assembler();
	
	void assemble();
	
private:
	template <class Key, class Value>
	bool is_present_in_map(const std::map<Key, Value> *m, const Key key);
	
	void handle_operand(const std::string instr_name);
	void reserve_space_for_label();
	void put_labels_in_reserved_spaces();
	void swap_endianness(int32_t* value);
	void error(std::string msg);
	void write_program();
	
	std::ifstream input;
	std::string dest_path;
	
	std::vector<uint8_t> program;
	
	const int32_t INITIAL_ADDRESS = -1;
	std::map<std::string, std::pair<std::vector<size_t>, int32_t>> labels;
	/*
	 * std::string - label's name;
	 * std::vector - vector of label usages' locations;
	 * int32_t - label declaration's address
	*/
	
	const std::map<std::string, uint8_t> instrs = 
	{
		{"NOP",    0x00},
		{"ADD",    0x01},
		{"SUB",    0x02},
		{"NEG",    0x03},
		{"SHL",    0x04},
		{"SHR",    0x05},
		{"AND",    0x06},
		{"OR",     0x07},
		{"XOR",    0x08},
		{"NOT",    0x09},
		{"JMP",    0x0A},
		{"JZ",     0x0B},
		{"JNZ",    0x0C},
		{"PUSH",   0x0D},
		{"RM",     0x0E},
		{"PUSHIP", 0x0F},
		{"POPIP",  0x10},
		{"RMIP",   0x11},
		{"PUSHPM", 0x12},
		{"POPPM",  0x13},
		{"INPUT",  0x14},
		{"PEEK",   0x15},
		{"HALT",   0x16}
	};
};

#endif // ASSEMBLER_H
