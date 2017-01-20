#include "Emulator.hpp"

#include <climits>
#include <fstream>
#include <iostream>
#include <stdexcept>

Emulator::Emulator(const std::string filename) :
	halt_called(false)
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

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		error("Failed to initialize SDL", false);

	const char *WIN_TITLE = "QProc Video Output";
	window = SDL_CreateWindow(WIN_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
								WINDOW_W, WINDOW_H, SDL_WINDOW_SHOWN);
	if (window == nullptr)
		error("Failed to create a video output window", false);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr)
		error("Failed to create a renderer", false);
}

Emulator::~Emulator()
{
	if (renderer != nullptr)
		SDL_DestroyRenderer(renderer);

	if (window != nullptr)
		SDL_DestroyWindow(window);

	SDL_Quit();
}

void Emulator::run()
{
	SDL_Event e;

	for (IP = 0; !halt_called && IP < PM_SIZE; IP++)
	{
		SDL_PollEvent(&e);

		draw_video_mem();
		do_instruction();
	}
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

void Emulator::do_instruction()
{
	enum opcodes
	{
		NOP = 0x00,
		ADD = 0x01,
		SUB = 0x02,
		NEG = 0x03,
		SHL = 0x04,
		SHR = 0x05,
		AND = 0x06,
		OR = 0x07,
		XOR = 0x08,
		NOT = 0x09,
		JMP = 0x0A,
		JZ = 0x0B,
		JNZ = 0x0C,
		PUSH = 0x0D,
		RM = 0x0E,
		PUSHIP = 0x0F,
		POPIP = 0x10,
		RMIP = 0x11,
		PUSHPM = 0x12,
		POPPM = 0x13,
		INPUT = 0x14,
		PEEK = 0x15,
		HALT = 0x16
	};

	switch (PM[IP])
	{
	case NOP:
		break;
	case ADD:
		DS.push(pop_DS() + pop_DS());
		break;
	case SUB:
	{
		const int32_t Y = pop_DS(), X = pop_DS();
		DS.push(X - Y);
	}
		break;
	case NEG:
		DS.push(-pop_DS());
		break;
	case SHL:
	{
		const int32_t Y = pop_DS(), X = pop_DS();
		DS.push(X << Y);
	}
		break;
	case SHR:
	{
		const int32_t Y = pop_DS(), X = pop_DS();
		DS.push(X >> Y);
	}
		break;
	case AND:
		DS.push(pop_DS() & pop_DS());
		break;
	case OR:
		DS.push(pop_DS() | pop_DS());
		break;
	case XOR:
		DS.push(pop_DS() ^ pop_DS());
		break;
	case NOT:
		DS.push(~pop_DS());
		break;
	case JMP:
	{
		const int32_t X = pop_DS();
		if (is_not_in_bounds(X))
			error("X is out of bounds in JMP", true);

		IP = X - 1; //IP will get incremented in a loop afterwards
	}
		break;
	case JZ:
	{
		const int32_t Y = pop_DS(), X = pop_DS();
		if (is_not_in_bounds(Y))
			error("Y is out of bounds in JZ", true);
		if (X == 0)
			IP = Y - 1; //IP will get incremented in a loop afterwards
	}
		break;
	case JNZ:
	{
		const int32_t Y = pop_DS(), X = pop_DS();
		if (is_not_in_bounds(Y))
			error("Y is out of bounds in JZ", true);
		if (X != 0)
			IP = Y - 1; //IP will get incremented in a loop afterwards
	}
		break;
	case PUSH:
		if (is_not_in_bounds(IP, PM_SIZE - sizeof(int32_t) - 1))
			error("PUSH used without a proper operand", true);
		DS.push(get_data_from_PM(IP + 1));
		IP += sizeof(int32_t); // IP will get incremented in a loop afterwards
		break;
	case RM:
		pop_DS();
		break;
	case PUSHIP:
	{
		const int32_t X = pop_DS();
		if (is_not_in_bounds(X))
			error("X is out of bounds in PUSHIP", true);
		IS.push(X);
	}
		break;
	case POPIP:
		IP = pop_IS();
		if (is_not_in_bounds(IP))
			error("IP is out of bounds in POPIP", true);
		IP--; // IP will get incremented in a loop afterwards
		break;
	case RMIP:
		pop_IS();
		break;
	case PUSHPM:
	{
		const int32_t X = pop_DS();
		if (is_not_in_bounds(X, PM_SIZE - sizeof(int32_t)))
			error("X is out of bounds in PUSHPM", true);
		DS.push(get_data_from_PM(X));
	}
		break;
	case POPPM:
	{
		int32_t Y = pop_DS();
		int32_t X = pop_DS();
		if (is_not_in_bounds(X, PM_SIZE - sizeof(int32_t)))
			error("X is out of bounds in POPPM", true);
		for (size_t i = 0; i < sizeof(int32_t); i++)
			PM[X++] = reinterpret_cast<int8_t*>(&Y)[sizeof(int32_t) - 1 - i];
	}
		break;
	case INPUT:
	{
		int32_t input;
		std::cin >> input;
		if (std::cin.fail())
			error("Invalid input", true);
		DS.push(input);
	}
		break;
	case PEEK:
		if (DS.empty())
			error("Tried to PEEK empty DS", true);
		std::cout << DS.top() << std::endl;
		break;
	case HALT:
		halt_called = true;
		break;
	default:
		error("Unknown instruction", true);
	}
}

uint8_t Emulator::get_bit(const uint8_t number, const size_t bit_num)
{
	return (number >> (bit_num - 1)) & 1;
}

void Emulator::draw_video_mem()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
	SDL_RenderClear(renderer);

	const size_t VIDEOMEM_SIZE = WINDOW_W * WINDOW_H;
	const size_t VIDEOMEM_BEG = PM_SIZE - VIDEOMEM_SIZE;

	for (int i = 0; i < VIDEOMEM_SIZE; i++)
	{
		const uint8_t pixel = PM[VIDEOMEM_BEG + i];

		const uint8_t intensity = 128 * get_bit(pixel, 4),
						r = intensity + 127 * get_bit(pixel, 3),
						g = intensity + 127 * get_bit(pixel, 2),
						b = intensity + 127 * get_bit(pixel, 1);

		SDL_SetRenderDrawColor(renderer, r, g, b, 0xFF);
		SDL_RenderDrawPoint(renderer, i % WINDOW_W, i / WINDOW_W);
	}

	SDL_RenderPresent(renderer);
}

void Emulator::error(const std::string msg, bool print_instr_number)
{
    std::string exception_text = "ERROR: " + msg;
    if (print_instr_number)
        exception_text += " ; Instruction #" + std::to_string(IP);

    throw std::runtime_error(exception_text);
}
