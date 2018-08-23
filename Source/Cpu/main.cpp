/*!
 * @file main.c
 * @brief Main program module.
 * @author Stanislau Shimovolos
 * @version 3.1
 * @date 2018-8-7
 */


#include "cpu.h"


int main(int argc, char *argv[])
{
    try
    {
        cpu megaXEON(XEON_RAM_SIZE, XEON_REGISTERS_AMOUNT);

        if (argc > 1)
            megaXEON.execute(argv[1]);
        else
            std::cout << "Expected executable file's name" << std::endl;
    }
    catch (std::exception &e)
    {
        std::cerr << e.what();
    }
    return 0;
}

