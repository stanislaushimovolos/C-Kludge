#include "Assembler.h"

const char inputFilename[] = "../Example/asmCode.txt";
const char outFilename[] = "../Example/machCode.txt";
const char labelName[] = ":";


int main()
{
    code_t code = {};

    getBuf(&code, inputFilename);

    makeTokens(&code);


    displayText(&code);

    //char **arrayPtrCmd = (char **) calloc(cmdAmount + 1, sizeof(char **));

    //char *labelArr = createPtrArrLabelArr(code, &arrayPtrCmd, &cmdAmount);

    //double *machCode = (double *) calloc(cmdAmount + 1, sizeof(double));


    //assemble(arrayPtrCmd, machCode, cmdAmount, labelArr);

    //writeCode(machCode, cmdAmount, outFilename);


    destructCode_t(&code);

    //free(machCode);
    //free(arrayPtrCmd);
    //free(labelArr);
    return 0;
}
