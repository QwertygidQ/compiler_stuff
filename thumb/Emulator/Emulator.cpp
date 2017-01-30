#include "Emulator.hpp"

#include <climits>
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
		case 0:		// Software Interrupt
			break;
		case 1:		// Add offset to stack pointer
			break;
		case 2:		// Hi register operations/branch exchange
			break;
		case 3:		// ALU operations
		{
			enum Ops
			{
				AND = 0b0000, EOR = 0b0001, LSL = 0b0010, LSR = 0b0011,
				ASR = 0b0100, ADC = 0b0101, SBC = 0b0110, ROR = 0b0111,
				TST = 0b1000, NEG = 0b1001, CMP = 0b1010, CMN = 0b1011,
				ORR = 0b1100, MUL = 0b1101, BIC = 0b1110, MVN = 0b1111
			};

			uint16_t op = get_bit_sequence(PM[pc], 9, 6);
			uint16_t rs = get_bit_sequence(PM[pc], 5, 3);
			uint16_t rd = get_bit_sequence(PM[pc], 2, 0);

			if (rd >= R_SIZE)
				error("RD is out of bounds", true);
			else if (rs >= R_SIZE)
				error("RS is out of bounds", true);

			switch (op)
			{
			case AND:
				r[rd] &= r[rs];
				break;
			case EOR:
				r[rd] ^= r[rs];
				break;
			case LSL:
				r[rd] <<= r[rs];
				break;
			case LSR:
				r[rd] >>= r[rs];
				break;
			case ASR:
				r[rd] = (int32_t) r[rd] >> r[rs];
				break;
			case ADC:
				r[rd] += r[rs] + get_bit(cpsr, Flags::C);
				break;
			case SBC:
				r[rd] -= r[rs] + get_bit(cpsr, Flags::C);
				break;
			case ROR:
				break;
			case TST:
				break;
			case NEG:
				break;
			case CMP:
				break;
			case CMN:
				break;
			case ORR:
				break;
			case MUL:
				break;
			case BIC:
				break;
			case MVN:
				break;
			default:
				error("Emulator bug, invalid operation in ALU operations", true);
			}

			if (op != TST && op != CMP && op != CMN)
			{
				change_bit(cpsr, Flags::N, get_bit(r[rd], 31));
				change_bit(cpsr, Flags::Z, r[rd] == 0);
			}
		}
			break;
		case 4:		// Add/subtract
		{
			uint8_t i = get_bit(PM[pc], 10);
			uint8_t op = get_bit(PM[pc], 9);
			uint16_t argument = get_bit_sequence(PM[pc], 8, 6);
			uint16_t rs = get_bit_sequence(PM[pc], 5, 3);
			uint16_t rd = get_bit_sequence(PM[pc], 2, 0);

			if (rd >= R_SIZE)
				error("RD is out of bounds", true);
			else if (rs >= R_SIZE)
				error("RS is out of bounds", true);
			else if (!i && argument >= R_SIZE)
				error("RN/Offset3 is out of bounds", true);

			if (!op)
			{
				if (!i) // ADD (register)
				{
					change_bit(cpsr, Flags::C, r[rs] > UINT32_MAX - r[argument]);
					change_bit(cpsr, Flags::V, r[rs] > INT32_MAX - r[argument]);

					r[rd] = r[rs] + r[argument];
				}
				else //ADD (immediate)
				{
					change_bit(cpsr, Flags::C, r[rs] > UINT32_MAX - argument);
					change_bit(cpsr, Flags::V, r[rs] > INT32_MAX - argument);

					r[rd] = r[rs] + argument;
				}
			}
			else
			{
				if (!i) // SUB (register)
				{
					change_bit(cpsr, Flags::C, r[rs] < r[argument]);
					change_bit(cpsr, Flags::V, r[rs] < INT32_MIN + r[argument]);

					r[rd] = r[rs] - r[argument];
				}
				else // SUB (immediate)
				{
					change_bit(cpsr, Flags::C, r[rs] < argument);
					change_bit(cpsr, Flags::V, r[rs] < INT32_MIN + argument);

					r[rd] = r[rs] - argument;
				}
			}

			change_bit(cpsr, Flags::N, get_bit(r[rd], 31));
			change_bit(cpsr, Flags::Z, r[rd] == 0);
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
			enum Ops
			{
				LSL = 0b00, LSR = 0b01, ASR = 0b10
			};

			uint16_t op = get_bit_sequence(PM[pc], 12, 11);
			uint16_t offset5 = get_bit_sequence(PM[pc], 10, 6);
			uint16_t rs = get_bit_sequence(PM[pc], 5, 3);
			uint16_t rd = get_bit_sequence(PM[pc], 2, 0);

			if (rd >= R_SIZE)
				error("RD is out of bounds", true);
			else if (rs >= R_SIZE)
				error("RS is out of bounds", true);

			change_bit(cpsr, Flags::N, 0);
			change_bit(cpsr, Flags::Z, 0);
			change_bit(cpsr, Flags::C, 0);

			switch (op)
			{
			case LSL:
				r[rd] = r[rs] << offset5;

				if (offset5 > 0)
					change_bit(cpsr, Flags::C, get_bit(r[rs], 31 - offset5 + 1));
				break;
			case LSR:
				r[rd] = r[rs] >> offset5;

				if (offset5 > 0)
					change_bit(cpsr, Flags::C, get_bit(r[rs], offset5 - 1));
				break;
			case ASR:
				r[rd] = (int32_t)r[rs] >> offset5;

				if (offset5 > 0)
					change_bit(cpsr, Flags::C, get_bit(r[rs], offset5 - 1));
			default:
				error("Unknown instruction of the 'Move shifted register' format type", true);
			}

			if (offset5 > 0)
			{
				change_bit(cpsr, Flags::N, get_bit(r[rd], 31));
				change_bit(cpsr, Flags::Z, r[rd] == 0);
			}

		}
			break;
		case 17:	// Move/compare/add/subtract immediate
		{
			enum Ops
			{
				MOV = 0b00, CMP = 0b01, ADD = 0b10, SUB = 0b11
			};

			uint16_t op = get_bit_sequence(PM[pc], 12, 11);
			uint16_t rd = get_bit_sequence(PM[pc], 10, 8);
			uint16_t offset8 = get_bit_sequence(PM[pc], 7, 0);

			if (rd >= R_SIZE)
				error("RD is out of bounds", true);

			switch (op)
			{
			case MOV:
				r[rd] = offset8;
				break;
			case CMP:
			{
				uint32_t tmp_result = r[rd] - offset8;

				change_bit(cpsr, Flags::N, get_bit(tmp_result, 31));
				change_bit(cpsr, Flags::Z, tmp_result == 0);
				change_bit(cpsr, Flags::C, r[rd] < offset8);
				change_bit(cpsr, Flags::V, r[rd] < INT32_MIN + offset8);
				break;
			}
			case ADD:
				r[rd] += offset8;

				change_bit(cpsr, Flags::N, get_bit(r[rd], 31));
				change_bit(cpsr, Flags::Z, r[rd] == 0);
				change_bit(cpsr, Flags::C, r[rd] > UINT32_MAX - offset8);
				change_bit(cpsr, Flags::V, r[rd] > INT32_MAX - offset8);
				break;
			case SUB:
				r[rd] -= offset8;

				change_bit(cpsr, Flags::N, get_bit(r[rd], 31));
				change_bit(cpsr, Flags::Z, r[rd] == 0);
				change_bit(cpsr, Flags::C, r[rd] < offset8);
				change_bit(cpsr, Flags::V, r[rd] < INT32_MAX + offset8);
				break;
			default:
				error("Emulator bug, invalid operation in move/cmp/add/sub immediate", true);
			}
		}
			break;
		case 18:	// Load/store with immediate offset
			break;
		default:
			error("Unknown format type", true);
		}
	}
}

uint16_t Emulator::get_bit_sequence(const uint16_t number, const int msb, const int lsb) const
{
	if (msb >= sizeof(uint16_t) * CHAR_BIT || msb < 0)
		error("MSB argument is out of range in get_bit_sequence()", true);
	if (lsb >= sizeof(uint16_t) * CHAR_BIT || lsb < 0)
		error("LSB argument is out of range in get_bit_sequence()", true);
	if (msb < lsb)
		error("MSB is less than LSB in get_bit_sequence()", true);

	int n = 1;
	for (int i = 0; i < msb - lsb; i++)
	{
		n <<= 1;
		n++;
	}

	return (number >> lsb) & n;
}

uint8_t Emulator::get_bit(const uint32_t number, const int bit) const
{
	if (bit >= sizeof(uint32_t) * CHAR_BIT || bit < 0)
		error("Bit argument is out of range in get_bit()", true);

	return (number >> bit) & 1;
}

void Emulator::change_bit(uint32_t &number, const int bit, const uint8_t new_val)
{
	if (bit >= sizeof(uint32_t) * CHAR_BIT || bit < 0)
		error("Bit argument is out of range in change_bit()", true);

	if (new_val)
		number |= (1 << bit);
	else
		number &= ~(1 << bit);
}

int Emulator::get_format_type(const uint16_t instr) const
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

void Emulator::error(const std::string msg, bool special_registers_dump) const
{
	std::string exception_text = "ERROR: " + msg;
	if (special_registers_dump)
		exception_text += "\nSP: " + std::to_string(sp) + "\nLR: " + std::to_string(lr) +
		"\nPC: " + std::to_string(pc) + "\nCPSR: " + std::to_string(cpsr);

	throw std::runtime_error(exception_text);
}