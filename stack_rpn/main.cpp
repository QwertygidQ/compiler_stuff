#include <cmath>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stack>

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

    std::cout << "Result: " << interpret(expr) << std::endl;

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
                error("Unknown character! Quitting...");
            }
            catch (const std::out_of_range&)
            {
                error("Number value is out of range! Quitting...");
            }
        }
    }
    
    if (stack.empty())
        error("Stack is empty! Quitting...");

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
