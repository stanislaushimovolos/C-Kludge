/*!
 * @file Assembler.c
 * @brief Implementation of @ref Assembler.h functions.
 * @author Stanislau Shimovolos
 * @version 3.1
 * @date 2018-7-19
 */


#include "Assembler.h"


#define ALLOC_CHECK(var, msg)                                                           \
if (!(var))                                                                             \
    return throw_error(MEMORY_ERR, "System couldn't allocate memory for the data.",     \
        msg, __PRETTY_FUNCTION__, __LINE__, __FILE__);


#define ARG_CHECK(var)                                                                  \
if (!(var))                                                                             \
    return throw_error(ARGUMENTS_ERR, "", "Unexpected values of arguments (nullptr)."   \
                                          , __PRETTY_FUNCTION__, __LINE__, __FILE__);   \


#define FILE_CHECK(var, name)                                                           \
if(!(var))                                                                              \
 {                                                                                      \
    printf("Source file name is %s\n",name);                                            \
    return throw_error(OPENFILE_ERR, "Can't open source file.",                         \
        "Input file error.",                                                            \
        __PRETTY_FUNCTION__, __LINE__, __FILE__);                                       \
}

#define throw(num, msg1, msg2)                                                          \
     return throw_error(num, msg1, msg2,                                                \
        __PRETTY_FUNCTION__, __LINE__, __FILE__);                                       \



#define execute(_func_)         \
do                              \
{                               \
    state = _func_;             \
    if (state)                  \
    {                           \
        destructCode_t(data);   \
        return state;           \
    }                           \
}while(0)


int assemble(code_t *data, int argc, const char *argv[])
{
    ARG_CHECK(data);
    argc--;

/*!
 * @warning Do not rename @ref state (macro definition).
 */
    int state = 0;

    if (argc)
        execute(getBuf(data, argv[1]));
    else
        execute(getBuf(data, defaultInput));
    argc--;

    processAsmCode(data, labelSymbol);
    compile(data);

    if (argc > 0)
        execute(writeCode(data, argv[2]));
    else
        execute(writeCode(data, defaultOutput));

    return 0;
}


#undef execute

int getBuf(code_t *data, const char *inputFile)
{
    assert(data);
    assert (inputFile);
    ARG_CHECK(data && inputFile);

    size_t l_buf_sz = 0;
    FILE *file = fopen(inputFile, "r");

    assert (file);
    FILE_CHECK(file, inputFile);

    fseek(file, 0, SEEK_END);
    l_buf_sz = (size_t) ftell(file);
    fseek(file, 0, SEEK_SET);

    data->buffer = (char *) calloc(l_buf_sz + 1, sizeof(char));

    assert(data->buffer);
    ALLOC_CHECK(data->buffer, "Buffer");

    data->size = l_buf_sz;
    fread(data->buffer, 1, l_buf_sz, file);
    fclose(file);

    return 0;
}


int processAsmCode(code_t *data, const char *label)
{
    assert (data);
    assert (data->buffer);
    ARG_CHECK(data && data->buffer);

    size_t _tokensNumber = countTokens(data->buffer);

    unsigned int *_labels = (unsigned int *) calloc(INIT_LABEL_NUMBER, sizeof(int));
    char **_tokensArr = (char **) calloc(_tokensNumber + 1, sizeof(char *));
    data->binaryCode = (double *) calloc(_tokensNumber + 1, sizeof(char *));

    assert(data->binaryCode);
    assert(_tokensArr);
    assert(_labels);

    ALLOC_CHECK(data->binaryCode, "Bin - code");
    ALLOC_CHECK(_labels, "Labels");
    ALLOC_CHECK(_tokensArr, "Tokens");

    unsigned int _labelNumber = INIT_LABEL_NUMBER;
    unsigned int tokCounter = 0;
    size_t curLabel = 0;

    char *curToken = strtok(data->buffer, "\n\t ");

    while (curToken != NULL)
    {
        if (strcmp(label, curToken) == 0)
        {
            curToken = strtok(NULL, "\n\t ");
            assert(curToken);

            if (!curToken)
                throw(LABEL_EOF, "Error, expected a number after the label symbol.", "");

            curLabel = strtoul(curToken, NULL, 10);

            if (!curLabel)
                throw (LABEL_VAL_ERR, "Error, expected a number after the label symbol.", "")

            curToken = strtok(NULL, "\n\t ");

            if (curLabel >= _labelNumber)
            {
                _labelNumber *= 2;
                _labels = (unsigned int *) realloc(_labels, _labelNumber * sizeof(int));

                assert(_labels);
                ALLOC_CHECK(_labels, "Labels reallocation");
            }

            if (!_labels[curLabel])
                _labels[curLabel] = tokCounter + 1;
            else
            {
                printf("Label number: %lu\n", curLabel);
                throw (LABEL_REPEAT, "Error, labels value was used twice.", "");
            }

            continue;
        }

        _tokensArr[tokCounter] = curToken;
        curToken = strtok(NULL, "\n\t ");
        tokCounter++;
    }

    data->labelsNumber = _labelNumber;
    data->tokens = _tokensArr;
    data->tokensNumber = tokCounter;
    data->labels = _labels;

    return 0;
}


int compile(code_t *data)
{
    assert(data);
    assert(data->tokens);
    assert(data->labels);
    assert(data->buffer);
    assert(data->binaryCode);

    ARG_CHECK(data &&
              data->tokens &&
              data->labels &&
              data->buffer &&
              data->binaryCode);

/*!
* @warning Do not rename these variables:
 * @ref codeCounter
 * @ref integerTemp
 * @ref doubleTemp
 * @ref tokens
 * @ref _labels
 * @ref tokensNumber
 * They are very important for code generation.
*/
    int codeCounter = 0;
    int integerTemp = 0;
    double doubleTemp = 0;

    char **tokens = data->tokens;
    unsigned int *_labels = data->labels;
    unsigned int tokensNumber = data->tokensNumber;

    while (tokens[codeCounter])
    {
        doubleTemp = 0;
        integerTemp = 0;


#define  DEF_CMD(name, num, codeAsm, codeCpu)           \
    if (strcmp (tokens[codeCounter], #name) == 0) \
    {                                                   \
        codeAsm;                                        \
    }

#include "../Commands.h"

        printf("Operand %s", tokens[codeCounter]);
        throw(UNKNOWN_OPERAND, "Operand is unknown", "");
    }

    return 0;
}

#undef DEF_CMD


void destructCode_t(code_t *data)
{
    if (data)
    {
        if (data->buffer)
            free(data->buffer);
        if (data->labels)
            free(data->labels);
        if (data->binaryCode)
            free(data->binaryCode);

        data->binaryCode = NULL;
        data->labels = NULL;
        data->buffer = NULL;
    }
}


int writeCode(code_t *data, const char *outFilename)
{
    assert (outFilename);
    ARG_CHECK(outFilename);

    FILE *outFile = fopen(outFilename, "w");

    assert(outFile);
    FILE_CHECK(outFile, outFilename);

    fprintf(outFile, "%s\n%s %s\n", SIGNATURE, ASM_NAME, ASM_VERSION);

    int codeSz = data->tokensNumber;
    double *_binaryCode = data->binaryCode;

    for (int i = 0; i < codeSz; i++)
        fprintf(outFile, "%lg ", _binaryCode[i]);

    return 0;
}


int displayTokens(code_t *data)
{
    assert (data);
    assert(data->tokens);

    ARG_CHECK(data && data->tokens);

    char **_tokens = data->tokens;
    int _tokensNum = data->tokensNumber;

    for (int i = 0; i < _tokensNum; i++)
        printf("%s\n", _tokens[i]);

    return 0;
}


int dumpCode_t(code_t *data)
{
    assert(data);
    ARG_CHECK(data);

    printf("Tokens:\n");
    if (data->buffer)
        displayTokens(data);

    if (data->labels)
    {
        int _labelNumber = data->labelsNumber;
        printf("Labels:\n");

        for (int i = 0; i < _labelNumber; i++)
            printf("№%d Lvalue = %d\n", i, data->labels[i]);
    }

    printf("Number of tokens: %d\n"
           "Number of labels: %d\n",
           data->tokensNumber,
           data->labelsNumber);

    return 0;
}


size_t countTokens(const char *buffer)
{
    assert(buffer);

    size_t tokensCounter = 0;
    int i = 0;

    while (buffer[i])
    {
        if (buffer[i] != '\n' &&
            buffer[i] != '\t' &&
            buffer[i] != ' ')
        {
            tokensCounter++;
            i++;
            {
                while (buffer[i] && buffer[i] != '\n' &&
                       buffer[i] != '\t' &&
                       buffer[i] != ' ')
                    i++;
            }
        }
        i++;
    }

    return tokensCounter;
}


int throw_error(unsigned int err_num, const char *usr_msg, const char *err_msg, const char *_func, int _line,
                const char *_file)
{
    if (usr_msg && usr_msg[0])
        printf("%s\n", usr_msg);

    if ((err_num < OUT_OF_ERR_RANGE_ERR) && err_msg && _func && _file)
    {
        if (err_msg[0])
            fprintf(stderr, "%s\n", err_msg);
        fprintf(stderr, "Error № %d: %s\n"
                        "In function: %s\n"
                        "Line %d\n"
                        "File: %s\n\n", err_num, errList[err_num], _func, _line, _file);

        return err_num;
    }
    else return OUT_OF_ERR_RANGE_ERR;
}

#undef ALLOC_CHECK
#undef ARG_CHECK
