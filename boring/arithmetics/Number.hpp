#ifndef NUMBER_HPP
#define NUMBER_HPP

#include <string>

class Number
{
public:
    Number();
    Number(std::string num);

    friend Number operator+(const Number &number1, const Number &number2);
    friend Number operator-(const Number &number1, const Number &number2);
private:
    Number(const std::string num, bool is_negative);

    bool is_valid_num(std::string num) const;

    void make_equal_length(std::string &number1, std::string &number2) const;

    inline int char_to_digit(char c) const;
    inline char digit_to_char(int i) const;

    std::string number;
    bool is_negative;
};

#endif
