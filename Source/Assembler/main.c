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

    //dumpCode_t(&code);

    destructCode_t(&code);

    return 0;
}
