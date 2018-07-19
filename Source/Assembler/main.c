/*!
 * @file main.c
 * @brief Main program module.
 * @author Stanislau Shimovolos
 * @version 3.1
 * @date 2018-7-19
 */

#include "Assembler.h"

int main(int argc, const char *argv[])
{
    code_t code = {};
    int err_num = assemble(&code, argc, argv);

    destructCode_t(&code);
    return err_num;
}
