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
    
    size_t pop_IS();
    int32_t pop_DS();
    
    void swap_endianness(int32_t *value);
    
    bool is_not_in_bounds(int32_t value);
    
    void run();
    
    void error(std::string msg);
    
private:
    static const size_t PM_SIZE = 1024;
    int32_t PM[PM_SIZE] = {0};
    
    size_t IP = 0;
    std::stack<size_t> IS;
    
    std::stack<int32_t> DS;
};

#endif
