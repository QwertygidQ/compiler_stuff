#ifndef STACK_HPP
#define STACK_HPP

#include <cstdlib>
#include <string>


template <typename T>
class Stack
{
private:
    int start_canary;
public:
    size_t max_size;

    Stack(const size_t max_size);
    ~Stack();

    bool empty() const;
    size_t size() const;

    void push(T element);
    T top() const;
    T pop();

    void dump() const;

private:
    void error(std::string error_msg, bool dump_needed) const;
    T calculate_checksum() const;
    void verify_checksum() const;

    T checksum;

    size_t cur_size;
    T *mem;
};

#include "Stack.inl"

#endif
