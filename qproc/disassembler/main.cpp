#include <iostream>
#include <cstdlib>
#include <stdexcept>

#include "Disassembler.hpp"

int main()
{
    std::cout << std::endl << "QProc disassembler" <<
        std::endl << "Qwertygid, 2016" << std::endl << std::endl;

    std::cout << "Enter ROM's location: ";

    std::string rom_path;
    std::cin >> rom_path;

    std::cout << "Enter resulting file's location: ";
    std::string result_path;
    std::cin >> result_path;

    std::cout << std::endl;

    try
    {
        Disassembler disassembler(rom_path, result_path);
        disassembler.run();
    }
    catch (const std::runtime_error &ex)
    {
        std::cerr << std::endl << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << std::endl << "Success!" << std::endl;

    return EXIT_SUCCESS;
}
