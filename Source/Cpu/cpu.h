/*!
 * @file cpu.h
 * @brief Contains @ref cpu class.
 * @author Stanislau Shimovolos
 * @version 3.1
 * @date 2018-8-7
 */


#ifndef VCPU_CPU_H
#define VCPU_CPU_H

#define COMMANDS_INIT_AMOUNT 50
#define RETURNS_INIT_AMOUNT 25

#define XEON_REGISTERS_AMOUNT 4
#define XEON_RAM_SIZE 256

#include "stack.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

/**
* @brief The list of possible errors
*/
enum error_codes
{
    READ_ERROR = 1,                 //!< Means that some problem was detected in process of opening file.
    TOKEN_ERROR                     //!< Means that processor met unknown operand
};


/*!
      @brief A class which simulates work of stack machine.
      @warning Do not rename @ref cpu class members: they are very important for code generation.
 */

class cpu
{
private:

    stack<int> _returns;            //!<Return addresses of functions
    stack<double> values;           //!<Processor's stack segment

    vector<double> _registers;      //!<Processor's registers (8 byte)
    vector<double> _ram;            //!<Processor's RAM
    vector<double> _commands;       //!<Processor's code segment

    size_t _sizeOfCode;             //!<Size of executable file
    size_t _ramSize;                //!<Size of RAM (in bytes)
    size_t _regAmount;              //!<Number of processor's registers

public:

    size_t getRam();

    /*!
     @brief Execute current program in @ref  _commands.
     @return Error code.
     */
    int execute(std::string &&filename);

    /*!
    @brief Loads data from a "binary file" .
    @param[in] filename Name of "binary file".
    @return Error's code.
    */
    int loadData(std::string &&filename);

    /*!
    @brief A constructor. Takes number of registers  and size of RAM.
    @param[in] ramSize Size of processor's RAM (in bytes).
    @param[in] registersAmount Number of processor's registers.
    */
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


/*!
 * Uses for code generation.
 */
#define  DEF_CMD(name, num, codeAsm, codeCpu)           \
            case num:                                   \
                                                        \
            {                                            \
                codeCpu;                                \
            }

int cpu::execute(std::string &&filename)
{
    //Upload binary into processor's memory
    auto status = loadData(std::move(filename));
    if (status)
        return status;

    /*!
    @warning Do not rename @ref counter (Instruction pointer at @ref commands.h).
    */
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

    //Read all file
    fin.seekg(0, std::ios::end);

    long size = fin.tellg();
    std::string data((size_t) size, ' ');
    fin.seekg(0);
    fin.read(&data[0], size);
    fin.close();

    std::string token;
    std::istringstream tokenStream(data);

    //Pointer at current token an pointer to it
    char *end_ptr;
    double cur_token = 0;

    while (std::getline(tokenStream, token, ' '))
    {
        cur_token = strtod(token.c_str(), &end_ptr);

        // if token is not a number => return error
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

size_t cpu::getRam()
{
    return _ramSize;
}

#endif
