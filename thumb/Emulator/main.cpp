#include "Emulator.hpp"

#include <iostream>
#include <stdexcept>

int main()
{
	try
	{
		Emulator emulator("input.bin");
		emulator.run();
	}
	catch (const std::runtime_error &ex)
	{
		std::cerr << ex.what() << std::endl;
		return EXIT_FAILURE;
	}
	catch (const std::exception &ex)
	{
		std::cerr << "An unusual error occured: " <<
			std::endl << ex.what() << std::endl;
		return EXIT_FAILURE;
	}
	catch (const std::string &str)
	{
		std::cerr << "An unusual error occured: " <<
			std::endl << str << std::endl;
		return EXIT_FAILURE;
	}
	catch (const char* str)
	{
		std::cerr << "An unusual error occured: " <<
			std::endl << str << std::endl;
		return EXIT_FAILURE;
	}
	catch (...)
	{
		std::cerr << "A very unusual error occured" <<
			std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}