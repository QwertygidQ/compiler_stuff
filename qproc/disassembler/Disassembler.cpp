#include "Disassembler.hpp"

#include <climits>
#include <iostream>
#include <map>
#include <stdexcept>

Disassembler::Disassembler(std::string rom_path, std::string res_path) : ip(0)
{
    std::ifstream rom_ifs(rom_path, std::ios::binary | std::ios::ate);
    if (!rom_ifs.is_open())
        error("Failed to open the ROM file", false);

    std::streampos filesize = rom_ifs.tellg();

    if (filesize > sizeof(uint8_t) * MAX_ROM_SIZE)
        error("Corrupt ROM", false);

    rom_ifs.seekg(0);

    size_t cell = 0;
    while (rom_ifs.tellg() != filesize)
        rom_ifs.read(reinterpret_cast<char*>(&ROM[cell++]), sizeof(uint8_t));

    rom_ifs.close();

    res_ofs.open(res_path);
    if (!res_ofs.is_open())
        error("Failed to open the resulting file", false);
}

Disassembler::~Disassembler()
{
    if (res_ofs.is_open())
        res_ofs.close();
}

void Disassembler::run()
{
    static const std::map<uint8_t, std::string> instrs =
	{
		{0x00,    "NOP"},
		{0x01,    "ADD"},
		{0x02,    "SUB"},
		{0x03,    "NEG"},
		{0x04,    "SHL"},
		{0x05,    "SHR"},
		{0x06,    "AND"},
		{0x07,    "OR"},
		{0x08,    "XOR"},
		{0x09,    "NOT"},
		{0x0A,    "JMP"},
		{0x0B,    "JZ"},
		{0x0C,    "JNZ"},
		{0x0D,    "PUSH"},
		{0x0E,    "RM"},
		{0x0F,    "PUSHIP"},
		{0x10,    "POPIP"},
		{0x11,    "RMIP"},
		{0x12,    "PUSHPM"},
		{0x13,    "POPPM"},
		{0x14,    "INPUT"},
		{0x15,    "PEEK"},
		{0x16,    "HALT"}
};

    for (; ip < MAX_ROM_SIZE; ip++)
    {
        if (instrs.find(ROM[ip]) != instrs.end())
        {
            bool newline_needed = ip != MAX_ROM_SIZE - 1;

            res_ofs << instrs.at(ROM[ip]);
            if (instrs.at(ROM[ip]) == "PUSH")
            {
                if (ip + 1 > MAX_ROM_SIZE - sizeof(int32_t))
                    error("PUSH used without a proper operand", true);

                res_ofs << " " << construct_integer(ip + 1);
                ip += 4;
            }
            else if (instrs.at(ROM[ip]) == "HALT")
                break;

            if (newline_needed)
                res_ofs << std::endl;
        }
        else
            error("Unknown operation", true);
    }
}

void Disassembler::error(std::string err_msg, bool print_ip) const
{
    std::string ex_text = "ERROR: " + err_msg;

    if (print_ip)
        ex_text += "; Instruction #" + std::to_string(ip);

    throw std::runtime_error(ex_text);
}

int32_t Disassembler::construct_integer(size_t start_pos) const
{
    if (start_pos > MAX_ROM_SIZE - sizeof(int32_t))
        error("Not enough space for an integer", true);

    int32_t integer = 0;

    size_t bits = sizeof(uint8_t) * CHAR_BIT;

    size_t i = 0;
    for (int j = sizeof(uint32_t) - 1; j >= 0; j--, i++)
        integer |= ROM[start_pos + i] << (bits * j);

    return integer;
}
