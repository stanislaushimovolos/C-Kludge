#include "Assembler.h"


int getBuf(code_t *data, const char *inputFile)
{
    assert(data);
    assert (inputFile);


    size_t l_buf_sz = 0;
    FILE *file = fopen(inputFile, "r");
    assert (file);


    fseek(file, 0, SEEK_END);
    l_buf_sz = (size_t) ftell(file);
    fseek(file, 0, SEEK_SET);

    data->buffer = (char *) calloc(l_buf_sz + 1, sizeof(char));
    assert(data->buffer);

    data->size = l_buf_sz;

    fread(data->buffer, 1, l_buf_sz, file);
    fclose(file);

    return 0;
}

size_t countTokens(const char *buffer)
{
    size_t tokensCounter = 0;
    int i = 0;

    while (buffer[i])
    {
        if (buffer[i] == '\n' ||
            buffer[i] == '\t' ||
            buffer[i] == ' ')
            tokensCounter++;
        i++;
    }

    return tokensCounter;
}


int makeTokens(code_t *data)
{

    assert (data);
    assert (data->buffer);

    size_t _tokensNumber = countTokens(data->buffer);

    char **_tokensArr = (char **) calloc(_tokensNumber + 1, sizeof(char *));
    unsigned int *_labels = (unsigned int *) calloc(INIT_LABEL_NUMBER, sizeof(int));

    assert(_tokensArr);
    assert(_labels);

    unsigned int _labelNumber = INIT_LABEL_NUMBER;
    unsigned int tokCounter = 0;
    size_t curLabel = 0;

    char *curToken = strtok(data->buffer, "\n\t ");

    while (curToken != NULL)
    {
        if (strcmp(":", curToken) == 0)
        {
            curToken = strtok(NULL, "\n\t ");
            curLabel = strtoul(curToken, NULL, 10);
            curToken = strtok(NULL, "\n\t ");

            if (curLabel >= _labelNumber)
            {
                _labelNumber *= 2;
                _labels = (unsigned int *) realloc(_labels, _labelNumber * sizeof(int));
            }

            _labels[curLabel] = tokCounter + 1;
            continue;
        }

        _tokensArr[tokCounter] = curToken;
        curToken = strtok(NULL, "\n\t ");
        tokCounter++;
    }

    data->labelsNum = _labelNumber;
    data->tokens = _tokensArr;
    data->tokensNum = tokCounter;
    data->labels = _labels;

    return 0;
}


void destructCode_t(code_t *data)
{
    if (data)
    {
        if (data->buffer)
            free(data->buffer);

        if (data->labels)

            data->labels = 0;
        data->buffer = NULL;
    }
}

int dumpCode_t(code_t *data)
{
    assert(data);
    printf();

}


int displayText(code_t *data)
{
    assert (data);
    assert(data->tokens);

    char **arrOfStr = data->tokens;


    for (int i = 0; i < data->tokensNum; i++)
        printf("%s\n", arrOfStr[i]);

    return 0;

}


void assemble(char **const _arrayPtrCmd, double *const machCode, size_t sizeOfMachCode, char *_labelArr)
{

    assert (machCode);
    assert (_arrayPtrCmd);

    int codeCounter = 0;

    while (_arrayPtrCmd[codeCounter] != 0 && codeCounter < sizeOfMachCode)
    {

#define  DEF_CMD(name, num, codeAsm, codeCpu)\
    if (strcmp (_arrayPtrCmd[codeCounter], #name) == 0){\
                                                        \
                                                        \
        codeAsm                                         \
                                                        \
    }

#include "Commands.h"

        printf("WRONG INPUT");
        exit(EXIT_FAILURE);
    }
}

#undef DEF_CMD


void *writeCode(const double *machCode, const size_t sizeOfMachCode, const char *_outFilename)
{

    assert (machCode);

    FILE *outFile = fopen(_outFilename, "w");

    fprintf(outFile, "%s\n", SIGNATURE);
    fprintf(outFile, "%s ", ASM_NAME);
    fprintf(outFile, "%s\n", ASM_VERSION);
    for (int i = 0; i < sizeOfMachCode; i++)
    {

        fprintf(outFile, "%lg ", machCode[i]);
    }
}
