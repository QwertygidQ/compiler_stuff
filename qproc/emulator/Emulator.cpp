#include "Emulator.hpp"

#include <fstream>
#include <iostream>

Emulator::Emulator(const std::string filename)
{    
    std::ifstream file;
    file.open(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        error("Could not open the file");
    
    std::streampos fsize = file.tellg();
    if (fsize % sizeof(int32_t) != 0 || fsize > PM_SIZE * sizeof(int32_t))
        error("Corrupt ROM");
    
    file.seekg(0);
    
    size_t PM_cell = 0;
    while (file.tellg() != fsize)
    {
        file.read(reinterpret_cast<char*>(&PM[PM_cell]), sizeof(int32_t));
        swap_endianness(&PM[PM_cell]);
        PM_cell++;
    }

    file.close();
}

size_t Emulator::pop_IS()
{
    if (IS.empty())
        error("Attempted to pop empty IS");
    
    size_t top = IS.top();
    IS.pop();
    
    return top;
}

int32_t Emulator::pop_DS()
{
    if (DS.empty())
        error("Attempted to pop empty DS");
    
    size_t top = DS.top();
    DS.pop();
    
    return top;
}

void Emulator::swap_endianness(int32_t *value)
{
    *value = (*value >> 24) | ((*value >> 8) & 0xFF00) |
                ((*value << 8) & 0xFF0000) | (*value << 24);
}

bool Emulator::is_not_in_bounds(int32_t value)
{
    return value < 0 || value >= PM_SIZE;
}

void Emulator::run()
{       
    for (IP = 0; IP < PM_SIZE; IP++)
        switch (PM[IP])
        {
            case 0x00000000: // NOP
                break;
            case 0x00000001: // ADD
                DS.push(pop_DS() + pop_DS());
                break;
            case 0x00000002: //SUB
                {
                    const int32_t Y = pop_DS(), X = pop_DS();
                    DS.push(X - Y);
                }
                break;
            case 0x00000003: //NEG
                DS.push(-pop_DS());
                break;
            case 0x00000004: //SHL
                {
                    const int32_t Y = pop_DS(), X = pop_DS();
                    DS.push(X << Y);
                }
                break;
            case 0x00000005: //SHR
                {
                    const int32_t Y = pop_DS(), X = pop_DS();
                    DS.push(X >> Y);
                }
                break;
            case 0x00000006: //AND
                DS.push(pop_DS() & pop_DS());
                break;
            case 0x00000007: //OR
                DS.push(pop_DS() | pop_DS());
                break;
            case 0x00000008: //XOR
                DS.push(pop_DS() ^ pop_DS());
                break;
            case 0x00000009: //NOT
                DS.push(!pop_DS());
                break;
            case 0x0000000A: //JMP
                {
                    const int32_t X = pop_DS();
                    if (is_not_in_bounds(X))
                        error("X is out of bounds in JMP");
                    IP = X - 1; // IP will get incremented in this loop afterwards
                }
                break;
            case 0x0000000B: //JZ
                {
                    const int32_t Y = pop_DS(), X = pop_DS();
                    if (is_not_in_bounds(Y))
                        error("Y is out of bounds in JZ");
                    if (X == 0)
                        IP = X - 1; // IP will get incremented in this loop afterwards
                }
                break;
            case 0x0000000C: //JNZ
                {
                    const int32_t Y = pop_DS(), X = pop_DS();
                    if (is_not_in_bounds(Y))
                        error("Y is out of bounds in JZ");
                    if (X != 0)
                        IP = X - 1; // IP will get incremented in this loop afterwards
                }
                break;
            case 0x0000000D: //PUSH
                if (IP == PM_SIZE - 1)
                    error("PUSH used without an operand");
                DS.push(PM[++IP]);
                break;
            case 0x0000000E: //RM
                pop_DS();
                break;
            case 0x0000000F: //PUSHIP
                IS.push(IP);
                break;
            case 0x00000010: //POPIP
                IP = pop_IS();
                if (is_not_in_bounds(IP))
                    error("IP is out of bounds after POPIP");
                break;
            case 0x00000011: //RMIP
                pop_IS();
                break;
            case 0x00000012: //PUSHPM
                {
                    const int32_t X = pop_DS();
                    if (is_not_in_bounds(X))
                        error("X is out of bounds in PUSHPM");
                    DS.push(PM[X]);
                }
                break;
            case 0x00000013: //POPPM
                {
                    const int32_t Y = pop_DS(), X = pop_DS();
                    if (is_not_in_bounds(X))
                        error("X is out of bounds in PUSHPM");
                    PM[X] = Y;
                }
                break;
            case 0x00000014: //INPUT
                {
                    int32_t input;
                    std::cin >> input;
                    DS.push(input);
                }
                break;
            case 0x00000015: //PEEK
                std::cout << DS.top() << std::endl;
                break;
            case 0x00000016: //HALT
                return;
            default:
                error("Unknown operation");
        }
}

void Emulator::error(const std::string msg)
{
    std::cerr << msg << std::endl;
    exit(EXIT_FAILURE);
}
