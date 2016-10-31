#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stack>
#include <vector>

enum class Type
{
    Number, Plus, Minus, Mul, Div
};

struct Token
{
    Token(const Type _type, const double _value = 0.0) :
        type(_type), value(_value)
    {}

    const Type type;
    const double value;
};

void tokenize(std::vector<Token> *tokens, const std::string expr);
double interpret(const std::vector<Token> *tokens);
void error(const std::string message);

template <class Func>
void apply_op_to_stack(std::stack<double> *stack, const Func f);

int main()
{
    std::cout << "Enter a RPN expression: " << std::endl;

    std::string expr;
    std::getline(std::cin, expr);

    std::vector<Token> vec;
    tokenize(&vec, expr);

    std::cout << "Result: " << interpret(&vec) << std::endl;

    return EXIT_SUCCESS;
}

void tokenize(std::vector<Token> *tokens, const std::string expr)
{
    std::stringstream ss;
    ss.str(expr);
    
    std::string temp;
    while (ss >> temp)
    {
        if (temp == "+")
            tokens -> push_back(Token(Type::Plus));
        else if (temp == "-")
            tokens -> push_back(Token(Type::Minus));
        else if (temp == "*")
            tokens -> push_back(Token(Type::Mul));
        else if (temp == "/")
            tokens -> push_back(Token(Type::Div));
        else
        {
            double num;
            try
            {
                num = std::stod(temp);
            }
            catch (const std::invalid_argument&)
            {
                error("Unknown character! Quitting...");
            }
            catch (const std::out_of_range&)
            {
                error("Number value is out of range! Quitting...");
            }
            
            tokens -> push_back(Token(Type::Number, num));
        }
    }
}

double interpret(const std::vector<Token> *tokens)
{
    std::stack<double> stack;

    for (Token token : *tokens)
        switch (token.type)
        {
        case Type::Plus:
            apply_op_to_stack(&stack, std::plus<double>());
            break;
        case Type::Minus:
            apply_op_to_stack(&stack, std::minus<double>());
            break;
        case Type::Mul:
            apply_op_to_stack(&stack, std::multiplies<double>());
            break;
        case Type::Div:
            apply_op_to_stack(&stack, std::divides<double>());
            break;
        case Type::Number:
            stack.push(token.value);
            break;
        default:
            error("Unknown command! Quitting...");
        }

    return stack.top();
}

void error(const std::string message)
{
	std::cerr << message << std::endl;
	exit(EXIT_FAILURE);
}

template <class Func>
void apply_op_to_stack(std::stack<double> *stack, const Func f)
{
    if (stack -> size() < 2)
        error("Stack is too small to perform an operation on! Quitting...");

    double top = stack -> top();
    stack -> pop();

    double temp = f(stack -> top(), top);
    if (std::isinf(temp))
        error("Division by zero attempt! Quitting...");

    stack -> top() = temp;
}
