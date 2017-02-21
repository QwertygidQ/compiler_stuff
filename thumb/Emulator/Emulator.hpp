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
	int get_format_type(const uint16_t instr) const;

	uint32_t ror(const uint32_t number, const unsigned int len) const;

	void error(const std::string msg, bool register_dump) const;

	const int NEGATIVE_BIT = 31;

	static const size_t PM_SIZE = 204800;
	uint16_t PM[PM_SIZE] = { 0 };

	static const size_t R_SIZE = 8; //R0 - R7
	uint32_t r[R_SIZE] = { 0 };

	uint32_t &sp = r[5], &lr = r[6], &pc = r[7];

    enum Flags
	{
		N = 31, Z = 30, C = 29, V = 28
	};

	static const size_t CPSR_SIZE = 4;
	uint8_t cpsr[CPSR_SIZE];

	void LSL_imm5(const uint16_t offset5, const uint16_t rs, const uint16_t rd);
	void LSR_imm5(const uint16_t offset5, const uint16_t rs, const uint16_t rd);
	void ASR_imm5(const uint16_t offset5, const uint16_t rs, const uint16_t rd);

	void ADD_lo(const uint16_t rn, const uint16_t rs, const uint16_t rd);
	void ADD_imm3(const uint16_t offset3, const uint16_t rs, const uint16_t rd);
	void SUB_lo(const uint16_t rn, const uint16_t rs, const uint16_t rd);
	void SUB_imm3(const uint16_t offset3, const uint16_t rs, const uint16_t rd);

	void MOV_imm8(const uint16_t rd, const uint16_t offset8);
	void CMP_imm8(const uint16_t rd, const uint16_t offset8);
	void ADD_imm8(const uint16_t rd, const uint16_t offset8);
	void SUB_imm8(const uint16_t rd, const uint16_t offset8);

	void AND_lo(const uint16_t rs, const uint16_t rd);
	void EOR_lo(const uint16_t rs, const uint16_t rd);
	void LSL_lo(const uint16_t rs, const uint16_t rd);
	void LSR_lo(const uint16_t rs, const uint16_t rd);
	void ASR_lo(const uint16_t rs, const uint16_t rd);
	void ROR_lo(const uint16_t rs, const uint16_t rd);
	void TST_lo(const uint16_t rs, const uint16_t rd);
	void NEG_lo(const uint16_t rs, const uint16_t rd);
	void CMP_lo(const uint16_t rs, const uint16_t rd);
	void CMN_lo(const uint16_t rs, const uint16_t rd);
	void ORR_lo(const uint16_t rs, const uint16_t rd);
	void MUL_lo(const uint16_t rs, const uint16_t rd);
	void BIC_lo(const uint16_t rs, const uint16_t rd);
	void MVN_lo(const uint16_t rs, const uint16_t rd);
};

#endif
