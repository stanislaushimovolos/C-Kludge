
/*!
 * @file sorter.h
 * @brief Main program module.
 * @author Stanislau Shimovolos
 * @version 1.4
 * @date 2018-7-19
 */

#include "Assembler.h"

const char inputFilename[] = "../Example/asmCode.txt";
const char outFilename[] = "../Example/machCode.txt";
const char labelName[] = ":";


int main()
{
    code_t code = {};
    getBuf(&code, inputFilename);
    processAsmCode(&code, labelSign);
    assemble(&code);
    writeCode(&code, outFilename);
    destructCode_t(&code);

    return 0;
}
