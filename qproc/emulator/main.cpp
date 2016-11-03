#include "Emulator.hpp"

#include <iostream>

int main()
{
    std::cout << std::endl << "QProc emulator" << std::endl <<
        "Qwertygid, 2016" << std::endl << std::endl;
        
    std::cout << "Enter ROM filepath: ";
    
    std::string filename;
    std::cin >> filename;
    
    std::cout << std::endl;
    
    Emulator emulator(filename);
    emulator.run();
    
    return EXIT_SUCCESS;
}
