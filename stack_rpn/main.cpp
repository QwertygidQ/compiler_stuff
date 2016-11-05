#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stack>
#include <stdexcept>

double interpret(const std::string expr);
void error(const std::string message);

template <class Func>
void apply_op_to_stack(std::stack<double> *stack, const Func f);

int main()
{
    std::cout << std::endl << "RPN stack machine emulator" <<
        std::endl << "Qwertygid, 2016" << std::endl << std::endl;
        
    std::cout << "Enter a RPN expression: " << std::endl;

    std::string expr;
    std::getline(std::cin, expr);

    try
    {
        std::cout << "Result: " << interpret(expr) << std::endl;
    }
    catch (const std::runtime_error &ex)
    {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

double interpret(const std::string expr)
{
    std::stringstream ss;
    ss.str(expr);
    
    std::stack<double> stack;
    
    std::string next_str;
    while (ss >> next_str)
    {
        if (next_str == "+")
            apply_op_to_stack(&stack, std::plus<double>());
        else if (next_str == "-")
            apply_op_to_stack(&stack, std::minus<double>());
        else if (next_str == "*")
            apply_op_to_stack(&stack, std::multiplies<double>());
        else if (next_str == "/")
            apply_op_to_stack(&stack, std::divides<double>());
        else
        {
            double num;
            try
            {
                num = std::stod(next_str);
                stack.push(num);
            }
            catch (const std::invalid_argument&)
            {
                error("Unknown character");
            }
            catch (const std::out_of_range&)
            {
                error("Number value is out of range");
            }
        }
    }
    
    if (stack.empty())
        error("Stack is empty");

    return stack.top();
}

void error(const std::string message)
{
	throw std::runtime_error(message);
}

template <class Func>
void apply_op_to_stack(std::stack<double> *stack, const Func f)
{
    if (stack -> size() < 2)
        error("Stack is too small to perform an operation on");

    double top = stack -> top();
    stack -> pop();

    double temp = f(stack -> top(), top);
    if (std::isinf(temp))
        error("Division by zero attempt");

    stack -> top() = temp;
}
