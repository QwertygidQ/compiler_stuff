#include <iostream>
#include <stack>
#include <vector>
#include <functional>
#include <cstdlib>

enum class Type
{
    Number, Plus, Minus, Mul, Div
};

struct Token
{
    Token(const Type _type, const int _value = 0) :
        type(_type), value(_value)
    {}

    const Type type;
    const int value;
};

void tokenize(std::vector<Token> *tokens, std::string expr);
int interpret(std::vector<Token> *tokens);

template <class Func>
void apply_op_to_stack(std::stack<int> *stack, Func f);

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

void tokenize(std::vector<Token> *tokens, std::string expr)
{
    for (size_t i = 0; i < expr.size(); i++)
        switch (expr[i])
        {
        case '+':
            tokens -> push_back(Token(Type::Plus));
            break;
        case '-':
            tokens -> push_back(Token(Type::Minus));
            break;
        case '*':
            tokens -> push_back(Token(Type::Mul));
            break;
        case '/':
            tokens -> push_back(Token(Type::Div));
            break;
        default:
            if (isdigit(expr[i]))
            {
                int num = 0;

                size_t j = i;
                while (isdigit(expr[j]))
                {
                    num *= 10;
                    num += expr[j] - '0';

                    j++;
                }

                tokens -> push_back(Token(Type::Number, num));

                i = j - 1;
            }
            else if (isspace(expr[i]))
            {
                while (isspace(expr[i]))
                    i++;

                i--;
            }
            else
            {
                std::cerr << "Unknown character! Quitting..." << std::endl;
                exit(EXIT_FAILURE);
            }
        }
}

int interpret(std::vector<Token> *tokens)
{
    std::stack<int> stack;

    for (Token token : *tokens)
        switch (token.type)
        {
        case Type::Plus:
            apply_op_to_stack(&stack, std::plus<int>());
            break;
        case Type::Minus:
            apply_op_to_stack(&stack, std::minus<int>());
            break;
        case Type::Mul:
            apply_op_to_stack(&stack, std::multiplies<int>());
            break;
        case Type::Div:
            apply_op_to_stack(&stack, std::divides<int>());
            break;
        case Type::Number:
            stack.push(token.value);
            break;
        default:
            std::cerr << "Unknown command! Quitting..." << std::endl;
            exit(EXIT_FAILURE);
        }

    return stack.top();
}

template <class Func>
void apply_op_to_stack(std::stack<int> *stack, Func f)
{
    int top = stack -> top();
    stack -> pop();
    stack -> top() = f(stack -> top(), top);
}
