/*!
 * @file main.c
 * @brief Main program module.
 * @author Stanislau Shimovolos
 * @version 3.1
 * @date 2018-8-7
 */


#include <iostream>
#include "cpu.h"

const char defaultInput[] = "machCode.txt";

int main(int argc, char *argv[])
{
    try
    {
        cpu megaXEON(RAM_SIZE, REGISTERS_AMOUNT);

        // if source file is not opened => status = err_num
        int status = 0;
        if (argc > 1)
            status = megaXEON.loadData(argv[1]);
        else
            status = megaXEON.loadData(defaultInput);
        if (status)
            return status;

        megaXEON.execute();
    }

    catch (std::exception &e)
    {
        std::cerr << e.what();
    }
    return 0;
}

