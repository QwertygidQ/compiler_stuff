#include "Assembler.hpp"

#include <map>
#include <iostream>
#include <stdexcept>

Assembler::Assembler(std::string source_path, std::string dest_path)
{
	input.open(source_path);
	if (!input.is_open())
		error("Failed to open source code");
		
	output.open(dest_path, std::ios::binary | std::ios::trunc);
	if (!output.is_open())
		error("Failed to open ROM");
		
	current_memory_location = 0;
}

Assembler::~Assembler()
{
	if (input.is_open())
	{
		input.close();
		if (output.is_open())
			output.close();
	}
}

void Assembler::assemble()
{
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
	
	while (!input.eof())
	{
		std::string next;
		input >> next;
		
		try
		{
			 uint8_t value = instrs.at(next);
			output.write(reinterpret_cast<char*>(&value), sizeof(uint8_t));
			if (next == "PUSH")
			{
				std::string integer_str;
				input >> integer_str;
				if (input.eof())
					error("PUSH used without an operand");
				
				try
				{
					int32_t integer = std::stoi(integer_str);
					swap_endianness(&integer);
					output.write(reinterpret_cast<char*>(&integer), sizeof(int32_t));
				}
				catch (const std::invalid_argument&)
				{
					error("An integer or a label name should come after PUSH");
				}
				catch (const std::out_of_range&)
				{
					error("Integer is too big");
				}
			}
		}
		catch (const std::out_of_range&) // next is not in instrs map
		{
			if (next.empty())
				error("Got an empty string");
				
			if (next == "CALL")
			{
				//ADD CALL FUNCTIONALITY!!!
			}
			else if (next.back() == ':') // label declaration
			{
				next.pop_back();
				if (next == "CALL")
					error("Label cannot be named one of the reserved words");
				try
				{
					instrs.at(next);
					error("Label cannot be named one of the reserved words");
				}
				catch (const std::out_of_range&)
				{
					//ADD LABEL DECLARATION FUNCTIONALITY!!!
				}
			}
			//else if (*comment*) ADD COMMENT FUNCTIONALITY!!!
			else
				error("Unknown symbol");
		}
	}
}

void Assembler::swap_endianness(int32_t* value) // Change, perhaps?
{
	*value = (*value >> 24) | ((*value >> 8) & 0xFF00) |
		((*value << 8) & 0xFF0000) | (*value << 24);
}

void Assembler::error(std::string msg)
{
	throw std::runtime_error(msg);
}