#include "Emulator.hpp"

#include <iostream>
#include <limits>
#include <stdexcept>

int main(int argc, char *argv[])
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
    catch (const std::exception &ex)
    {
        std::cerr << "An unusual error occured: "
            << std::endl << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (const char *str)
    {
        std::cerr << "An unusual error occured: "
            << std::endl << str << std::endl;
        return EXIT_FAILURE;
    }
    catch (const std::string &str)
    {
        std::cerr << "An unusual error occured: "
            << std::endl << str << std::endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        std::cerr << "A VERY unusual error occured."
            << std::endl;
        return EXIT_FAILURE;
    }

	std::cout << std::endl << "The program has finished. Press ENTER to continue..." << std::endl;

	std::cin.ignore();
	std::cin.get();

    return EXIT_SUCCESS;
}
