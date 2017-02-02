#include "Emulator.hpp"

#include <bitset>
#include <climits>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
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
			uint16_t op = get_bit_sequence(PM[pc], 9, 6);
			uint16_t rs = get_bit_sequence(PM[pc], 5, 3);
			uint16_t rd = get_bit_sequence(PM[pc], 2, 0);

			if (rd >= R_SIZE)
				error("RD is out of bounds", true);
			else if (rs >= R_SIZE)
				error("RS is out of bounds", true);

			static const std::unordered_map<uint16_t, void(Emulator::*)(const uint16_t, const uint16_t)> operations =
			{
				{ 0b0000, &Emulator::AND_lo },
				{ 0b0001, &Emulator::EOR_lo },
				{ 0b0010, &Emulator::LSL_lo },
				{ 0b0011, &Emulator::LSR_lo },
				{ 0b0100, &Emulator::ASR_lo },
				{ 0b0101, &Emulator::ADC_lo },
				{ 0b0110, &Emulator::SBC_lo },
				{ 0b0111, &Emulator::ROR_lo },
				{ 0b1000, &Emulator::TST_lo },
				{ 0b1001, &Emulator::NEG_lo },
				{ 0b1010, &Emulator::CMP_lo },
				{ 0b1011, &Emulator::CMN_lo },
				{ 0b1100, &Emulator::ORR_lo },
				{ 0b1101, &Emulator::MUL_lo },
				{ 0b1110, &Emulator::BIC_lo },
				{ 0b1111, &Emulator::MVN_lo },
			};

			auto it = operations.find(op);
			if (it == operations.end())
				error("Unknown instruction of the 'ALU operations' format type", true);
			(this->*(it->second))(rs, rd);
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

			op <<= 1;
			op += i;

			static const std::unordered_map<uint16_t, void(Emulator::*)(const uint16_t, const uint16_t, const uint16_t)> operations =
			{
				{ 0b00, &Emulator::ADD_lo },
				{ 0b01, &Emulator::ADD_imm3 },
				{ 0b10, &Emulator::SUB_lo },
				{ 0b11, &Emulator::SUB_imm3 }
			};

			auto it = operations.find(op);
			if (it == operations.end())
				error("Unknown instruction of the 'Add/subtract' format type", true);
			(this->*(it->second))(argument, rs, rd);
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

			if (rd >= R_SIZE)
				error("RD is out of bounds", true);
			else if (rs >= R_SIZE)
				error("RS is out of bounds", true);

			static const std::unordered_map<uint16_t, void(Emulator::*)(const uint16_t, const uint16_t, const uint16_t)> operations =
			{
				{ 0b00, &Emulator::LSL_imm5 },
				{ 0b01, &Emulator::LSR_imm5 },
				{ 0b10, &Emulator::ASR_imm5 }
			};

			auto it = operations.find(op);
			if (it == operations.end())
				error("Unknown instruction of the 'Move shifted register' format type", true);
			(this->*(it->second))(offset5, rs, rd);
		}
			break;
		case 17:	// Move/compare/add/subtract immediate
		{
			uint16_t op = get_bit_sequence(PM[pc], 12, 11);
			uint16_t rd = get_bit_sequence(PM[pc], 10, 8);
			uint16_t offset8 = get_bit_sequence(PM[pc], 7, 0);

			if (rd >= R_SIZE)
				error("RD is out of bounds", true);

			static const std::unordered_map<uint16_t, void(Emulator::*)(const uint16_t, const uint16_t)> operations =
			{
				{ 0b00, &Emulator::MOV_imm8 },
				{ 0b01, &Emulator::CMP_imm8 },
				{ 0b10, &Emulator::ADD_imm8 },
				{ 0b11, &Emulator::SUB_imm8 }
			};

			auto it = operations.find(op);
			if (it == operations.end())
				error("Unknown instruction of the 'Move/compare/add/subtract immediate' format type", true);
			(this->*(it->second))(rd, offset8);
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

uint32_t Emulator::ror(const uint32_t number, const unsigned int len) const
{
	const unsigned int mask = CHAR_BIT * sizeof(uint32_t) - 1;

	if (len > mask)
		error("Attempted to shift a value by type width or more", true);

	return (number >> len) | (number << ((-len) & mask));
}

void Emulator::error(const std::string msg, bool register_dump) const
{
	std::string exception_text = "ERROR: " + msg;
	if (register_dump)
	{
		for (size_t i = 0; i < R_SIZE; i++)
			exception_text += "\nR" + std::to_string(i) + ": " + std::to_string(r[i]);

		exception_text += "\nSP: " + std::to_string(sp) + "\nLR: " + std::to_string(lr) +
			"\nPC: " + std::to_string(pc) + "\nCPSR: " + std::bitset<32>(cpsr).to_string();
	}

	throw std::runtime_error(exception_text);
}

void Emulator::LSL_imm5(const uint16_t offset5, const uint16_t rs, const uint16_t rd)
{
	r[rd] = r[rs] << offset5;

	change_bit(cpsr, Flags::N, get_bit(r[rd], NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, r[rd] == 0);
	if (offset5 > 0)
		change_bit(cpsr, Flags::C, get_bit(r[rs], 31 - offset5 + 1));

}

void Emulator::LSR_imm5(const uint16_t offset5, const uint16_t rs, const uint16_t rd)
{
	r[rd] = r[rs] >> offset5;

	change_bit(cpsr, Flags::N, get_bit(r[rd], NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, r[rd] == 0);
	if (offset5 > 0)
		change_bit(cpsr, Flags::C, get_bit(r[rs], offset5 - 1));
}

void Emulator::ASR_imm5(const uint16_t offset5, const uint16_t rs, const uint16_t rd)
{
	r[rd] = (int32_t)r[rs] >> offset5;

	change_bit(cpsr, Flags::N, get_bit(r[rd], NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, r[rd] == 0);
	if (offset5 > 0)
		change_bit(cpsr, Flags::C, get_bit(r[rs], offset5 - 1));
}

void Emulator::ADD_lo(const uint16_t rn, const uint16_t rs, const uint16_t rd)
{
	change_bit(cpsr, Flags::N, get_bit(r[rd], NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, r[rd] == 0);
	change_bit(cpsr, Flags::C, r[rs] > UINT32_MAX - r[rn]);
	change_bit(cpsr, Flags::V, r[rs] > INT32_MAX - r[rn]);

	r[rd] = r[rs] + r[rn];
}

void Emulator::ADD_imm3(const uint16_t offset3, const uint16_t rs, const uint16_t rd)
{
	change_bit(cpsr, Flags::N, get_bit(r[rd], NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, r[rd] == 0);
	change_bit(cpsr, Flags::C, r[rs] > UINT32_MAX - offset3);
	change_bit(cpsr, Flags::V, r[rs] > INT32_MAX - offset3);

	r[rd] = r[rs] + offset3;
}

void Emulator::SUB_lo(const uint16_t rn, const uint16_t rs, const uint16_t rd)
{
	change_bit(cpsr, Flags::N, get_bit(r[rd], NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, r[rd] == 0);
	change_bit(cpsr, Flags::C, r[rs] < r[rn]);
	change_bit(cpsr, Flags::V, r[rs] < INT32_MIN + r[rn]);

	r[rd] = r[rs] - r[rn];
}

void Emulator::SUB_imm3(const uint16_t offset3, const uint16_t rs, const uint16_t rd)
{
	change_bit(cpsr, Flags::N, get_bit(r[rd], NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, r[rd] == 0);
	change_bit(cpsr, Flags::C, r[rs] < offset3);
	change_bit(cpsr, Flags::V, r[rs] < INT32_MIN + offset3);

	r[rd] = r[rs] - offset3;
}

void Emulator::MOV_imm8(const uint16_t rd, const uint16_t offset8)
{
	r[rd] = offset8;

	change_bit(cpsr, Flags::N, get_bit(r[rd], NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, r[rd] == 0);
}

void Emulator::CMP_imm8(const uint16_t rd, const uint16_t offset8)
{
	change_bit(cpsr, Flags::C, r[rd] < offset8);
	change_bit(cpsr, Flags::V, r[rd] < INT32_MIN + offset8);

	uint32_t tmp_result = r[rd] - offset8;

	change_bit(cpsr, Flags::N, get_bit(tmp_result, NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, tmp_result == 0);
}

void Emulator::ADD_imm8(const uint16_t rd, const uint16_t offset8)
{
	change_bit(cpsr, Flags::C, r[rd] > UINT32_MAX - offset8);
	change_bit(cpsr, Flags::V, r[rd] > INT32_MAX - offset8);

	r[rd] += offset8;

	change_bit(cpsr, Flags::N, get_bit(r[rd], NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, r[rd] == 0);
}

void Emulator::SUB_imm8(const uint16_t rd, const uint16_t offset8)
{
	change_bit(cpsr, Flags::C, r[rd] < offset8);
	change_bit(cpsr, Flags::V, r[rd] < INT32_MIN + offset8);

	r[rd] -= offset8;

	change_bit(cpsr, Flags::N, get_bit(r[rd], NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, r[rd] == 0);
}

void Emulator::AND_lo(const uint16_t rs, const uint16_t rd)
{
	r[rd] &= r[rs];

	change_bit(cpsr, Flags::N, get_bit(r[rd], NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, r[rd] == 0);
}

void Emulator::EOR_lo(const uint16_t rs, const uint16_t rd)
{
	r[rd] ^= r[rs];

	change_bit(cpsr, Flags::N, get_bit(r[rd], NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, r[rd] == 0);
}

void Emulator::LSL_lo(const uint16_t rs, const uint16_t rd)
{
	if (r[rs] > 0)
		change_bit(cpsr, Flags::C, get_bit(r[rd], 31 - r[rs] + 1));

	r[rd] <<= r[rs];

	change_bit(cpsr, Flags::N, get_bit(r[rd], NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, r[rd] == 0);
}

void Emulator::LSR_lo(const uint16_t rs, const uint16_t rd)
{
	if (r[rs] > 0)
		change_bit(cpsr, Flags::C, get_bit(r[rd], r[rs] - 1));

	r[rd] >>= r[rs];

	change_bit(cpsr, Flags::N, get_bit(r[rd], NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, r[rd] == 0);
}

void Emulator::ASR_lo(const uint16_t rs, const uint16_t rd)
{
	if (r[rs] > 0)
		change_bit(cpsr, Flags::C, get_bit(r[rd], r[rs] - 1));

	r[rd] = (int32_t)r[rd] >> r[rs];

	change_bit(cpsr, Flags::N, get_bit(r[rd], NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, r[rd] == 0);
}

void Emulator::ADC_lo(const uint16_t rs, const uint16_t rd) // fix
{
	r[rd] += r[rs] + get_bit(cpsr, Flags::C);

	change_bit(cpsr, Flags::N, get_bit(r[rd], NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, r[rd] == 0);
}

void Emulator::SBC_lo(const uint16_t rs, const uint16_t rd) // fix
{
	r[rd] -= r[rs] + get_bit(cpsr, Flags::C);

	change_bit(cpsr, Flags::N, get_bit(r[rd], NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, r[rd] == 0);
}

void Emulator::ROR_lo(const uint16_t rs, const uint16_t rd)
{
	if (r[rs] > 0)
		change_bit(cpsr, Flags::C, get_bit(r[rd], r[rs] - 1));

	r[rd] = ror(r[rd], r[rs]);

	change_bit(cpsr, Flags::N, get_bit(r[rd], NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, r[rd] == 0);
}

void Emulator::TST_lo(const uint16_t rs, const uint16_t rd)
{
	uint32_t tmp_result = r[rd] & r[rs];

	change_bit(cpsr, Flags::N, get_bit(tmp_result, NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, tmp_result == 0);
}

void Emulator::NEG_lo(const uint16_t rs, const uint16_t rd) // fix
{
	r[rd] = -r[rs];

	change_bit(cpsr, Flags::N, get_bit(r[rd], NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, r[rd] == 0);
}

void Emulator::CMP_lo(const uint16_t rs, const uint16_t rd)
{
	uint32_t tmp_result = r[rd] - r[rs];

	change_bit(cpsr, Flags::N, get_bit(tmp_result, NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, tmp_result == 0);
	change_bit(cpsr, Flags::C, r[rd] < r[rs]);
	change_bit(cpsr, Flags::V, r[rd] < INT32_MIN + r[rs]);
}

void Emulator::CMN_lo(const uint16_t rs, const uint16_t rd)
{
	uint32_t tmp_result = r[rd] - (~r[rs]);

	change_bit(cpsr, Flags::N, get_bit(tmp_result, NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, tmp_result == 0);
	change_bit(cpsr, Flags::C, r[rd] < r[rs]);
	change_bit(cpsr, Flags::V, r[rd] < INT32_MIN + r[rs]);
}

void Emulator::ORR_lo(const uint16_t rs, const uint16_t rd)
{
	r[rd] |= r[rs];

	change_bit(cpsr, Flags::N, get_bit(r[rd], NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, r[rd] == 0);
}

void Emulator::MUL_lo(const uint16_t rs, const uint16_t rd)
{
	r[rd] *= r[rs];

	change_bit(cpsr, Flags::N, get_bit(r[rd], NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, r[rd] == 0);
}

void Emulator::BIC_lo(const uint16_t rs, const uint16_t rd)
{
	r[rd] &= ~r[rs];

	change_bit(cpsr, Flags::N, get_bit(r[rd], NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, r[rd] == 0);
}

void Emulator::MVN_lo(const uint16_t rs, const uint16_t rd)
{
	r[rd] = ~r[rs];

	change_bit(cpsr, Flags::N, get_bit(r[rd], NEGATIVE_BIT));
	change_bit(cpsr, Flags::Z, r[rd] == 0);
}
