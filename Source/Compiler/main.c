#include "RecursiveDescent/Descent.h"
#include "AsmTranslation/ToAsm.h"

const char defaultOutput[] = "out.txt";

int main(int argc, char *argv[])
{
    parser pars = {};
    int status = 0;

    if (argc > 1)
    {
        status = constructParser(&pars, argv[1]);
        argc--;
    }
    else
    {
        printf("Expected input file name\n");
        return 1;
    }
    if (status)
        return status;

    status = getTree(&pars);
    if (status)
    {
        destructParser(&pars);
        return status;
    }
    if (argc > 1)
        compile(&pars, argv[2]);
    else
        compile(&pars, defaultOutput);

    destructParser(&pars);
    return 0;
}