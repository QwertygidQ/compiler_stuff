#include <iostream>
#include <stdexcept>

#define DEFAULT_CANARY 1337
#define CANARY_STATUS(canary_val) (canary_val == DEFAULT_CANARY ? "OK" : "FAILURE")

template <typename T>
Stack<T>::Stack(const size_t max_size)
    : start_canary(DEFAULT_CANARY), max_size(max_size), cur_size(0)
{
    try
    {
        mem = new T[max_size + 1];
    }
    catch (const std::bad_alloc&)
    {
        error("Memory allocation failure", false);
    }

    mem[max_size] = DEFAULT_CANARY;
}

template <typename T>
Stack<T>::~Stack()
{
    delete[] mem;
}

template <typename T>
bool Stack<T>::empty() const
{
    return size == 0;
}

template <typename T>
size_t Stack<T>::size() const
{
    return size;
}

template <typename T>
void Stack<T>::push(T element)
{
    if (cur_size == max_size)
        error("Attempted to push out of stack bounds", true);

    mem[cur_size++] = element;
}

template <typename T>
T Stack<T>::top() const
{
    if (cur_size == 0)
        error("Attempted to access an empty stack", true);

    return mem[cur_size - 1];
}

template <typename T>
T Stack<T>::pop()
{
    if (cur_size == 0)
        error("Attempted to access an empty stack", true);

    return mem[--cur_size];
}

template <typename T>
void Stack<T>::dump() const
{
    std::cerr << "---------STACK DUMP BEGINNING---------" << std::endl;
    std::cerr << "Stack address: " << this << std::endl;
    std::cerr << "Stack memory address: " << mem << std::endl;
    std::cerr << "Stack memory maximum size: " << max_size << std::endl;
    std::cerr << "Stack memory current size: " << cur_size << std::endl;
    std::cerr << "Stack memory elements: " << std::endl;

    for (size_t i = 0; i < max_size; i++)
    {
        if (i < cur_size)
            std::cerr << "[*] ";
        else
            std::cerr << "[ ] ";

        std::cerr << "(" << i << ") " << mem[i] << std::endl;
    }

    std::cerr << std::endl;
    std::cerr << "Default canary value: " << DEFAULT_CANARY << std::endl;
    std::cerr << "Start canary value: " << start_canary << " @ " << &start_canary <<
                "; Status: " << CANARY_STATUS(start_canary) << std::endl;
    std::cerr << "End canary value: " << mem[max_size] << " @ " << mem + max_size <<
                "; Status: " << CANARY_STATUS(mem[max_size]) << std::endl;

    std::cerr << "------------STACK DUMP END------------" << std::endl;
}

template <typename T>
void Stack<T>::error(std::string error_msg, bool dump_needed) const
{
    std::cerr << "ERROR: " << error_msg << std::endl << std::endl;

    if (dump_needed)
        dump();

    throw std::runtime_error("Stack error");
}
