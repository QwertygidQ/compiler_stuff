#include "Emulator.hpp"

#include <iostream>

int main()
{
    std::string filename;
    std::cin >> filename;
    
    Emulator emulator(filename);
    emulator.run();
    
    return EXIT_SUCCESS;
}
