#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <cstdint>
#include <cstdlib>
#include <string>

class Emulator
{
public:
	Emulator(const std::string filename);

	void run();

private:
	uint16_t get_bit_sequence(const uint16_t number, const int msb, const int lsb) const;
	uint8_t get_bit(const uint32_t number, const int bit) const;
	void change_bit(uint32_t &number, const int bit, const uint8_t new_val);
	int get_format_type(const uint16_t instr) const;

	void error(const std::string msg, bool special_registers_dump) const;

	enum Flags
	{
		N = 31, Z = 30, C = 29, V = 28
	};

	static const size_t PM_SIZE = 204800;
	uint16_t PM[PM_SIZE] = { 0 };

	static const size_t R_SIZE = 8; //R0 - R7
	uint32_t r[R_SIZE] = { 0 };

	uint32_t sp = 0, lr = 0, pc = 0, cpsr = 0;
};

#endif
