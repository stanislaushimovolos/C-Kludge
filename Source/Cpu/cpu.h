//
// Created by Tom on 28.10.2017.
//

#ifndef VCPU_CPU_H
#define VCPU_CPU_H

#define COMMANDS_INIT_AMOUNT 50
#define RETURNS_INIT_AMOUNT 25

#define REGISTERS_AMOUNT 4
#define RAM_SIZE 256

#include "stack.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>


enum error_codes
{
    READ_ERROR = 1,
    TOKEN_ERROR
};


class cpu
{
private:

    stack<int> _returns;
    stack<double> values;

    vector<double> _registers;
    vector<double> _ram;
    vector<double> _commands;

    size_t _sizeOfCode;
    size_t _ramSize;
    size_t _regAmount;

public:

    int execute();

    int loadData(std::string &&filename);

    cpu(size_t ramSize, size_t registersAmount);
};


cpu::cpu(size_t ramSize, size_t registersAmount) try :

        values(COMMANDS_INIT_AMOUNT),
        _returns(RETURNS_INIT_AMOUNT),
        _registers(registersAmount + 1),
        _regAmount(registersAmount),
        _ram(ramSize + 1),
        _ramSize(ramSize),
        _commands(),
        _sizeOfCode(0)
{}

catch (std::exception &e)
{
    throw;
}


#define  DEF_CMD(name, num, codeAsm, codeCpu)           \
            case num:                                   \
            {                                           \
                codeCpu;                                \
            }

int cpu::execute()
{
    int counter = 0;
    while (counter < _sizeOfCode)
    {
        switch ((int) _commands[counter])
        {

#include "../Commands.h"

            default:
                throw std::runtime_error(std::string("Error, unknown operand: ") +
                                         std::string(std::to_string(_commands[counter])));
        }
    }
}


int cpu::loadData(std::string &&filename)
{
    std::ifstream fin(filename, std::ios::in);
    if (!fin)
    {
        std::cout << "Couldn't open file: " << filename << std::endl;
        return READ_ERROR;
    }

    fin.seekg(0, std::ios::end);

    long size = fin.tellg();
    std::string data((size_t) size, ' ');
    fin.seekg(0);
    fin.read(&data[0], size);
    fin.close();

    std::string token;
    std::istringstream tokenStream(data);

    char *end_ptr;
    double cur_token = 0;
    while (std::getline(tokenStream, token, ' '))
    {
        cur_token = strtod(token.c_str(), &end_ptr);
        if (end_ptr != nullptr && *end_ptr)
        {
            std::cout << "Sorry, but " << token.c_str() << " is not a number" << std::endl;
            return TOKEN_ERROR;
        }
        _commands.push_back(cur_token);
    }
    _sizeOfCode = _commands.getSize();
    return 0;
}

#endif
