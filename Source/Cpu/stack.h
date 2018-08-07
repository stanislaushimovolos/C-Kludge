#pragma once

#include "vector.h"

template<typename Data_T>
class stack
{
public:

    explicit stack(size_t size = START_VECTOR_SZ);

    stack(stack<Data_T> &&that) noexcept;

    void dump(std::ostream &os = std::cout);

    int push(const Data_T &value);

    int pop(Data_T *var);

    int pop();

    int stackOK();

private:

    vector<Data_T> data;

    int state;
};


template<typename Data_T>
stack<Data_T>::stack(stack<Data_T> &&that) noexcept :
        data(std::move(that.data)),
        state(0)
{}


template<typename Data_T>
stack<Data_T>::stack(size_t size) try :
        data(size),
        state(0)
{}
catch (std::exception &e)
{
    throw;
}


template<typename Data_T>
int stack<Data_T>::stackOK()
{
    state = data.vectorOK();
    return state;
}


template<typename Data_T>
int stack<Data_T>::push(const Data_T &value)
{
    return data.push_back(value);
}


template<typename Data_T>
int stack<Data_T>::pop()
{
    return data.pop_back();
}


template<typename Data_T>
int stack<Data_T>::pop(Data_T *var)
{
    return data.pop_back(var);
}


template<typename Data_T>
void stack<Data_T>::dump(std::ostream &os)
{
    std::cout << "Current size: " << data.getSize() << std::endl;
    std::cout << "Max size: " << data.getCapacity() << std::endl;
    std::cout << "Elements: " << std::endl;
    data.dump(os);
}