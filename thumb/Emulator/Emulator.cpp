#include "Emulator.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <utility>

Emulator::Emulator(std::string filename)
{
	std::ifstream ifstr;
	ifstr.open(filename, std::ios::binary | std::ios::ate);
	if (!ifstr.is_open())
		error("Failed to open the input file", false);

	std::streampos fsize = ifstr.tellg();

	std::cout << "Filesize: " << fsize << " bytes" << std::endl << std::endl;

	if (fsize > PM_SIZE * sizeof(uint16_t) || fsize % sizeof(uint16_t) != 0)
		error("The input file is corrupt", false);

	ifstr.seekg(0);

	size_t PM_Cell = 0;
	while (ifstr.tellg() < fsize)
		ifstr.read(reinterpret_cast<char*>(&PM[PM_Cell++]), sizeof(uint16_t));

	ifstr.close();
}

void Emulator::run()
{
	enum Formats
	{

	};

	for (; pc < PM_SIZE; pc++)
	{
		switch (get_format_type(PM[pc]))
		{
		case 0:		// Software Interrupt
			break;
		case 1:		// Add offset to stack pointer
			break;
		case 2:		// Hi register operations/branch exchange
			break;
		case 3:		// ALU operations
		{
			uint16_t op = get_bit_sequence(PM[pc], 9, 6);
			uint16_t rs = get_bit_sequence(PM[pc], 5, 3);
			uint16_t rd = get_bit_sequence(PM[pc], 2, 0);
		}
			break;
		case 4:		// Add/subtract
		{
			uint8_t i = get_bit(PM[pc], 10);
			uint8_t op = get_bit(PM[pc], 9);
			uint16_t argument = get_bit_sequence(PM[pc], 8, 6);
			uint16_t rs = get_bit_sequence(PM[pc], 5, 3);
			uint16_t rd = get_bit_sequence(PM[pc], 2, 0);

			if (!op)
			{
				if (!i)
					r[rd] = r[rs] + r[argument];
				else
					r[rd] = r[rs] + argument;
			}
			else
			{
				if (!i)
					r[rd] = r[rs] - r[argument];
				else
					r[rd] = r[rs] - argument;
			}
		}
			break;
		case 5:		// PC-relative load
			break;
		case 6:		// Unconditional branch
			break;
		case 7:		// Load/store with register offset
			break;
		case 8:		// Load/store sign-extended byte/halfword
			break;
		case 9:		// Load/store halfword
			break;
		case 10:	// SP-relative load/store
			break;
		case 11:	// Load address
			break;
		case 12:	// Push/pop registers
			break;
		case 13:	// Multiple load/store
			break;
		case 14:	// Conditional branch
			break;
		case 15:	// Long branch with link
			break;
		case 16:	// Move shifted register
		{
			uint16_t op = get_bit_sequence(PM[pc], 12, 11);
			uint16_t offset5 = get_bit_sequence(PM[pc], 10, 6);
			uint16_t rs = get_bit_sequence(PM[pc], 5, 3);
			uint16_t rd = get_bit_sequence(PM[pc], 2, 0);

			switch (op)
			{
			case 0b00:
				r[rd] = r[rs] << offset5;
				break;
			case 0b01:
				r[rd] = r[rs] >> offset5;
				break;
			case 0b10:
				r[rd] = (int32_t)r[rs] >> offset5;
			}

		}
			break;
		case 17:	// Move/compare/add/subtract immediate
			break;
		case 18:	// Load/store with immediate offset
			break;
		default:
			error("Unknown format type", true);
		}
	}
}

void Emulator::error(const std::string msg, bool special_registers_dump)
{
	std::string exception_text = "ERROR: " + msg;
	if (special_registers_dump)
		exception_text += "\nSP: " + std::to_string(sp) + "\nLR: " + std::to_string(lr) +
							"\nPC: " + std::to_string(pc) + "\nCPSR: " + std::to_string(cpsr);

	throw std::runtime_error(exception_text);
}

uint16_t Emulator::get_bit_sequence(const uint16_t number, const int msb, const int lsb)
{
	int n = 1;
	for (int i = 0; i < msb - lsb; i++)
	{
		n <<= 1;
		n++;
	}

	return (number >> lsb) & n;
}

uint8_t Emulator::get_bit(const uint16_t number, const int bit)
{
	return (number >> bit) & 1;
}

int Emulator::get_format_type(const uint16_t instr)
{
	const size_t SIGNATURES_NUM = 19;
	const std::pair<uint16_t, int> signatures[] = // { signature, its length }
	{
		{ 0b11011111,	8 },	// #0; Software Interrupt
		{ 0b10110000,	8 },	// #1; Add offset to stack pointer
		{ 0b010001,		6 },	// #2; Hi register operations/branch exchange
		{ 0b010000,		6 },	// #3; ALU operations
		{ 0b00011,		5 },	// #4; Add/subtract
		{ 0b01001,		5 },	// #5; PC-relative load
		{ 0b11100,		5 },	// #6; Unconditional branch
		{ 0b0101,		4 },	// #7; Load/store with register offset (check if bit 9 is 0)
		{ 0b0101,		4 },	// #8; Load/store sign-extended byte/halfword (check if bit 9 is 1)
		{ 0b1000,		4 },	// #9; Load/store halfword
		{ 0b1001,		4 },	// #10; SP-relative load/store
		{ 0b1010,		4 },	// #11; Load address
		{ 0b1011,		4 },	// #12; Push/pop registers (check if bits 10 & 9 are 10)
		{ 0b1100,		4 },	// #13; Multiple load/store
		{ 0b1101,		4 },	// #14; Conditional branch
		{ 0b1111,		4 },	// #15; Long branch with link
		{ 0b000,		3 },	// #16; Move shifted register
		{ 0b001,		3 },	// #17; Move/compare/add/subtract immediate
		{ 0b011,		3 }		// #18; Load/store with immediate offset
	};

	for (int i = 0; i < SIGNATURES_NUM; i++)
	{
		if ((instr >> (15 - signatures[i].second + 1)) == signatures[i].first)
		{
			switch (signatures[i].first)
			{
			case 0b0101:
				if (get_bit(instr, 9) == 1)
					return i + 1;
				break;
			case 0b1011:
				if (get_bit_sequence(instr, 10, 9) != 0b10)
					continue;
			}

			return i;
		}
	}

	return -1;
}