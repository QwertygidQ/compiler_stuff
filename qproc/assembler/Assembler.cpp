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
	while (!input.eof())
	{
		std::string next;
		input >> next;

		if (is_present_in_map(&instrs, next)) // Is next an instruction?
		{
			program.push_back(instrs.at(next));

			if (next == "PUSH")
				handle_operand("PUSH");
		}
		else
		{
			if (next.empty())
				error("Got an empty string");
			else if (next == "CALL")
			{
				program.push_back(instrs.at("PUSH"));

				int32_t after_jmp_addr = program.size() + 3 * sizeof(uint8_t) + 2 * sizeof(int32_t);
				swap_endianness(&after_jmp_addr);
				const uint8_t *after_jmp_addr_uint8_t_repr = reinterpret_cast<const uint8_t*>(&after_jmp_addr);
				for (size_t i = 0; i < sizeof(int32_t); i++)
					program.push_back(after_jmp_addr_uint8_t_repr[i]);

				program.push_back(instrs.at("PUSHIP"));

				program.push_back(instrs.at("PUSH"));
				handle_operand("CALL");
				program.push_back(instrs.at("JMP"));
			}
			else if (next.back() == ':') // Is next a label declaration?
			{
				next.pop_back();

				if (next.empty())
					error("Colon should be preceeded with a label name");
				else if (next == "CALL" || is_present_in_map(&instrs, next)) // Is next an instruction?
					error("Label cannot be named one of the reserved words");
				else if (is_present_in_map(&labels, next)) // Does this label already exist?
				{
					auto &value_pair = labels.at(next);
					if (value_pair.second != INITIAL_ADDRESS)
						error("Label redeclaration");
					value_pair.second = program.size();
				}
				else
					labels.emplace(next,
						std::make_pair(std::vector<size_t>(), program.size()));
			}
			//else if (*comment*) ADD COMMENT FUNCTIONALITY!!!
			else
				error("Unknown symbol");
		}
	}

	put_labels_in_reserved_spaces();

	write_program();
}

template <class Key, class Value>
bool Assembler::is_present_in_map(const std::map<Key, Value> *m, const Key key)
{
	return m -> find(key) != m -> end();
}

void Assembler::handle_operand(const std::string instr_name)
{
	std::string operand_str;
	if (input.eof())
		error(instr_name + " used without an operand");
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
		if (is_present_in_map(&instrs, operand_str)) // Is operand_str an instruction?
			error("An instruction cannot be an operand to " + instr_name);
		else if (is_present_in_map(&labels, operand_str)) // Does this label already exist?
			labels.at(operand_str).first.push_back(program.size());
		else
			labels.emplace(operand_str,
				std::make_pair(std::vector<size_t>(1, program.size()),
				INITIAL_ADDRESS));

		reserve_space_for_label();
	}
	catch (const std::out_of_range&)
	{
		error("Integer is too big");
	}
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

void Assembler::swap_endianness(int32_t* value)
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
	const int MAX_PROG_SIZE = 16384;
	if (program.size() > MAX_PROG_SIZE)
		error("Resulting program's size is bigger than MAX_PROG_SIZE");

	std::ofstream output;
	output.open(dest_path, std::ios::binary | std::ios::trunc);
	if (!output.is_open())
		error("Could not open ROM");

	output.write(reinterpret_cast<char*>(program.data()), program.size());

	output.close();
}
