#ifndef DISASSEMBLER_HPP
#define DISASSEMBLER_HPP

#include <fstream>
#include <cstdint>
#include <cstdlib>
#include <string>

class Disassembler
{
public:
    Disassembler(std::string rom_path, std::string res_path);
    ~Disassembler();

    void run();

private:
    void error(std::string err_msg, bool print_ip) const;
    int32_t construct_integer(size_t start_pos) const;

    static const size_t MAX_ROM_SIZE = 4096;
    uint8_t ROM[MAX_ROM_SIZE] = {0};

    size_t ip;

    std::ofstream res_ofs;
};

#endif // DISASSEMBLER_HPP
