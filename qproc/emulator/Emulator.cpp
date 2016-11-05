#include "Emulator.hpp"

#include <climits>
#include <fstream>
#include <iostream>
#include <stdexcept>

Emulator::Emulator(const std::string filename)
{
    std::ifstream file;
    file.open(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        error("Could not open the ROM", false);
    
    std::streampos fsize = file.tellg();
    std::cout << "Filesize: " << fsize << " bytes" << std::endl << std::endl;
    if (fsize > PM_SIZE * sizeof(uint8_t))
        error("Corrupt ROM", false);
    
    file.seekg(0);
    
    size_t PM_Cell = 0;
    while (file.tellg() != fsize)
        file.read(reinterpret_cast<char*>(&PM[PM_Cell++]), sizeof(uint8_t));
    
    file.close();
}

size_t Emulator::pop_IS()
{
    if (IS.empty())
        error("Attempted to pop empty IS", true);
    
    size_t top = IS.top();
    IS.pop();
    
    return top;
}

int32_t Emulator::pop_DS()
{
    if (DS.empty())
        error("Attempted to pop empty DS", true);
    
    int32_t top = DS.top();
    DS.pop();
    
    return top;
}

int32_t Emulator::get_data_from_PM(const size_t beginning)
{
    if (is_not_in_bounds(beginning, PM_SIZE - sizeof(int32_t)))
        error("Variable beginning is out of bounds in get_data_from_PM", true);
    
    size_t bits = sizeof(uint8_t) * CHAR_BIT;
    
    int32_t result = 0;
    
    size_t i = 0;
    for (int j = sizeof(uint32_t) - 1; j >= 0; j--, i++)
        result |= PM[beginning + i] << (bits * j);
    
    return result;
}

bool Emulator::is_not_in_bounds(const int32_t value, const size_t right_bound)
{
    return (value > right_bound) || (value < 0);
}

void Emulator::run()
{
    for (IP = 0; IP < PM_SIZE; IP++)
        switch (PM[IP])
        {
            case 0x00: //NOP
                break;
            case 0x01: //ADD
                DS.push(pop_DS() + pop_DS());
                break;
            case 0x02: //SUB
                {
                    const int32_t Y = pop_DS(), X = pop_DS();
                    DS.push(X - Y);
                }
                break;
            case 0x03: //NEG
                DS.push(-pop_DS());
                break;
            case 0x04: //SHL
                {
                    const int32_t Y = pop_DS(), X = pop_DS();
                    DS.push(X << Y);
                }
                break;
            case 0x05: //SHR
                {
                    const int32_t Y = pop_DS(), X = pop_DS();
                    DS.push(X >> Y);
                }
                break;
            case 0x06: //AND
                DS.push(pop_DS() & pop_DS());
                break;
            case 0x07: //OR
                DS.push(pop_DS() | pop_DS());
                break;
            case 0x08: //XOR
                DS.push(pop_DS() ^ pop_DS());
                break;
            case 0x09: //NOT
                DS.push(~pop_DS());
                break;
            case 0x0A: //JMP
                {
                    const int32_t X = pop_DS();
                    if (is_not_in_bounds(X))
                        error("X is out of bounds in JMP", true);
                    
                    IP = X - 1; //IP will get incremented in this loop afterwards
                }
                break;
            case 0x0B: //JZ
                {
                    const int32_t Y = pop_DS(), X = pop_DS();
                    if (is_not_in_bounds(Y))
                        error("Y is out of bounds in JZ", true);
                    if (X == 0)
                        IP = Y - 1; //IP will get incremented in this loop afterwards
                }
                break;
            case 0x0C: //JNZ
                {
                    const int32_t Y = pop_DS(), X = pop_DS();
                    if (is_not_in_bounds(Y))
                        error("Y is out of bounds in JZ", true);
                    if (X != 0)
                        IP = Y - 1; //IP will get incremented in this loop afterwards
                }
                break;
            case 0x0D: //PUSH
                if (is_not_in_bounds(IP, PM_SIZE - sizeof(int32_t) - 1))
                    error("PUSH used without a proper operand", true);
                DS.push(get_data_from_PM(IP + 1));
                IP += sizeof(int32_t); // IP will get incremented in this loop afterwards
                break;
            case 0x0E: //RM
                pop_DS();
                break;
            case 0x0F: //PUSHIP
                IS.push(IP);
                break;
            case 0x10: //POPIP
                IP = pop_IS();
                if (is_not_in_bounds(IP))
                    error("IP is out of bounds in POPIP", true);
                break;
            case 0x11: //RMIP
                pop_IS();
                break;
            case 0x12: //PUSHPM
                {
                    const int32_t X = pop_DS();
                    if (is_not_in_bounds(X, PM_SIZE - sizeof(int32_t)))
                        error("X is out of bounds in PUSHPM", true);
                    DS.push(get_data_from_PM(X));
                }
                break;
            case 0x13: //POPPM
                {
                    int32_t Y = pop_DS();
                    int32_t X = pop_DS();
                    if (is_not_in_bounds(X, PM_SIZE - sizeof(int32_t)))
                        error("X is out of bounds in POPPM", true);
                    for (size_t i = 0; i < sizeof(int32_t); i++)
                        PM[X++] = reinterpret_cast<int8_t*>(&Y)[sizeof(int32_t) - 1 - i];
                }
                break;
            case 0x14: //INPUT
                {
                    int32_t input;
                    std::cin >> input;
                    if (std::cin.fail())
                        error("Invalid input", true);
                    DS.push(input);
                }
                break;
            case 0x15: //PEEK
                if (DS.empty())
                    error("Tried to PEEK empty DS", true);
                std::cout << DS.top() << std::endl;
                break;
            case 0x16: //HALT
                return;
            default:
                error("Unknown instruction", true);
        }
}

void Emulator::error(const std::string msg, bool print_instr_number)
{
    std::string exception_text = msg;
    if (print_instr_number)
        exception_text += " ; Instruction #" + std::to_string(IP);
        
    throw std::runtime_error(exception_text);
}
