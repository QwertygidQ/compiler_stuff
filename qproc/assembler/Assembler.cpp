#include "Assembler.hpp"

#include <iostream>
#include <stdexcept>

Assembler::Assembler(std::string source_path, std::string dest_path) :
	dest_path(dest_path)
{
	input.open(source_path);
	if (!input.is_open())
		error("Failed to open source code");
}

Assembler::~Assembler()
{
	if (input.is_open())
		input.close();
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
			uint8_t value = instrs.at(next); // Is next an instruction?
			program.push_back(value);
			
			if (next == "PUSH")
			{
				std::string operand_str;
				if (input.eof())
					error("PUSH used without an operand");
				input >> operand_str;
				
				try
				{
					int32_t integer = std::stoi(operand_str); // Is operand_str an integer?
					swap_endianness(&integer);
					uint8_t *integer_uint8_t_repr = reinterpret_cast<uint8_t*>(&integer);
					for (size_t i = 0; i < sizeof(int32_t); i++)
						program.push_back(integer_uint8_t_repr[i]);
				}
				catch (const std::invalid_argument&)
				{
					try
					{
						instrs.at(operand_str); // Is operand_str an instruction?
						error("An instruction cannot be an operand to PUSH");
					}
					catch (const std::out_of_range&)
					{
						try
						{
							auto &value_pair = labels.at(operand_str); // Does this label already exist?
							value_pair.first.push_back(program.size());
						}
						catch (const std::out_of_range&)
						{
							labels.emplace(operand_str,
								std::make_pair(std::vector<size_t>(1, program.size()),
								INITIAL_ADDRESS));
						}
						
						reserve_space_for_label();
					}
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
			else if (next.back() == ':') // Is next a label declaration?
			{
				next.pop_back();
				
				if (next.empty())
					error("Colon should be preceeded with a label name");
				else if (next == "CALL")
					error("Label cannot be named one of the reserved words");
					
				try
				{
					instrs.at(next); // Is next an instruction?
					error("Label cannot be named one of the reserved words");
				}
				catch (const std::out_of_range&)
				{
					try
					{
						auto &value_pair = labels.at(next); // Does this label already exist?
						if (value_pair.second != INITIAL_ADDRESS)
							error("Label redeclaration");
						value_pair.second = program.size();
					}
					catch (std::out_of_range&)
					{
						labels.emplace(next,
							std::make_pair(std::vector<size_t>(), program.size()));
					}
				}
			}
			//else if (*comment*) ADD COMMENT FUNCTIONALITY!!!
			else
				error("Unknown symbol");
		}
	}
	
	put_labels_in_reserved_spaces();
	
	write_program();
}

void Assembler::reserve_space_for_label()
{
	for (size_t i = 0; i < sizeof(int32_t); i += sizeof(uint8_t))
		program.push_back(0);
}

void Assembler::put_labels_in_reserved_spaces()
{
	for (auto &elem : labels)
	{
		if (elem.second.second == INITIAL_ADDRESS) // if label declaration's address == INITIAL_ADDRESS
			error("Label used without a declaration");
		
		swap_endianness(&elem.second.second);
		
		for (auto &usage : elem.second.first)
		{
			const uint8_t *value_uint8_t_repr = reinterpret_cast<const uint8_t*>(&elem.second.second);
			for (size_t i = 0; i < sizeof(int32_t); i++)
				program[usage + i] = value_uint8_t_repr[i];
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

void Assembler::write_program()
{	
	const int MAX_PROG_SIZE = 4096;
	if (program.size() > MAX_PROG_SIZE)
		error("Resulting program's size is bigger than MAX_PROG_SIZE");
		
	std::ofstream output;
	output.open(dest_path, std::ios::binary | std::ios::trunc);
	if (!output.is_open())
		error("Could not open ROM");
	
	output.write(reinterpret_cast<char*>(program.data()), program.size());
	
	output.close();
}