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
	for (; pc < PM_SIZE; pc++)
	{
		switch (get_format_type(PM[pc]))
		{
		case 0:
			break;
		case 1:
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		case 10:
			break;
		case 11:
			break;
		case 12:
			break;
		case 13:
			break;
		case 14:
			break;
		case 15:
			break;
		case 16:
			break;
		case 17:
			break;
		case 18:
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
		{ 0b11011111,	8 },
		{ 0b10110000,	8 },
		{ 0b010001,		6 },
		{ 0b010000,		6 },
		{ 0b00011,		5 },
		{ 0b01001,		5 },
		{ 0b11100,		5 },
		{ 0b0101,		4 },	// check if bit 9 is 0
		{ 0b0101,		4 },	// check if bit 9 is 1
		{ 0b1000,		4 },
		{ 0b1001,		4 },
		{ 0b1010,		4 },
		{ 0b1011,		4 },	// check if bits 10 & 9 are 10
		{ 0b1100,		4 },
		{ 0b1101,		4 },
		{ 0b1111,		4 },
		{ 0b000,		3 },
		{ 0b001,		3 },
		{ 0b011,		3 }
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