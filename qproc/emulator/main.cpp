#include "Emulator.hpp"

#include <iostream>
#include <stdexcept>

int main()
{
    std::cout << std::endl << "QProc emulator" << std::endl <<
        "Qwertygid, 2016" << std::endl << std::endl;
        
    std::cout << "Enter ROM filepath: ";
    
    std::string filename;
    std::cin >> filename;
    
    std::cout << std::endl;
    
    try
    {
        Emulator emulator(filename);
        emulator.run();
    }
    catch (const std::runtime_error &ex)
    {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
