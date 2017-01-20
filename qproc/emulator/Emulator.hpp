#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <SDL.h>

#include <cstdint>
#include <cstdlib>
#include <stack>
#include <string>

class Emulator
{
public:
    Emulator(const std::string filename);
	~Emulator();

    void run();

private:
    size_t pop_IS();
    int32_t pop_DS();

    int32_t get_data_from_PM(const size_t beginning);

    bool is_not_in_bounds(const int32_t value, const size_t right_bound = PM_SIZE - 1);

	void do_instruction();
	
	uint8_t get_bit(const uint8_t number, const size_t bit_num);
	void draw_video_mem();

    void error(const std::string msg, bool print_instr_number);

    static const size_t PM_SIZE = 204800;
    uint8_t PM[PM_SIZE] = {0};

    size_t IP = 0;
    std::stack<size_t> IS;

    std::stack<int32_t> DS;

	bool halt_called;

	static const int WINDOW_W = 320, WINDOW_H = 200;

	SDL_Window *window = nullptr;
	SDL_Renderer *renderer = nullptr;
};

#endif
