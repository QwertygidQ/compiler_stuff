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

	void error(const std::string msg, bool special_registers_dump);

private:
	uint16_t get_bit_sequence(const uint16_t number, const int msb, const int lsb);
	uint8_t get_bit(const uint16_t number, const int bit);
	int get_format_type(const uint16_t instr);

	static const size_t PM_SIZE = 204800;
	uint16_t PM[PM_SIZE] = { 0 };

	static const size_t R_SIZE = 8; //R0 - R7
	uint32_t r[R_SIZE] = { 0 };

	uint32_t sp = 0, lr = 0, pc = 0, cpsr = 0;
};

#endif
