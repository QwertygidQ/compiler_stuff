#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <cstdint>
#include <cstdlib>
#include <stack>
#include <string>

class Emulator
{
public:
    Emulator(const std::string filename);

    void run();

private:
    size_t pop_IS();
    int32_t pop_DS();

    int32_t get_data_from_PM(const size_t beginning);

    bool is_not_in_bounds(const int32_t value, const size_t right_bound = PM_SIZE - 1);

    void error(const std::string msg, bool print_instr_number);

    static const size_t PM_SIZE = 16384;
    uint8_t PM[PM_SIZE] = {0};

    size_t IP = 0;
    std::stack<size_t> IS;

    std::stack<int32_t> DS;
};

#endif
