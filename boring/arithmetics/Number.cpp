#include "Number.hpp"

#include <cstdlib>
#include <algorithm>

Number::Number() : number("0"), is_negative(false)
{}

Number::Number(std::string num)
{
    if (!is_valid_num(num))
    {} //error

    if (number[0] == '-')
    {
        is_negative = true;
        num.erase(num.begin());
    }
    else
        is_negative = false;

    number = std::string(num.end(), num.begin());
}

Number(const std::string num, bool is_negative) : is_negative(is_negative)
{
    if (!is_valid_num(num))
    {} //error
    else if (number[0] == '-')
    {} //error

    number = num;
}

friend Number Number::operator+(const Number &number1, const Number &number2)
{
    if (number1.is_negative)
        return operator-(number2, Number(number1.number, false));
    else if (number2.is_negative)
        return operator-(number1, Number(number2.number, false));

    make_

    std::string result = "";

}

friend Number Number::operator-(const Number &number1, const Number &number2)
{
}


bool Number::is_valid_num(std::string num) const
{}

void make_equal_length(std::string &number1, std::string &number2) const
{
    if (number1.size() > number2.size())
        number2.append(number1.size() - number2.size(), '0');
    else if (number.size() < num.size())
        number1.append(number2.size() - number1.size(), '0');
}

inline int Number::char_to_digit(char c) const
{
    return c - '0';
}

inline char Number::digit_to_char(int i) const
{
    if (i < 0 || i > 9)
    {} //error

    return i + '0';
}
