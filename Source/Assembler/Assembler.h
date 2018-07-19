//
// Created by superstraz on 7/19/18.
//

#ifndef ASSEMBLER_ASSEMBLER_H
#define ASSEMBLER_ASSEMBLER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/// Release version so asserts were turned off.
#define NDEBUG

#include <assert.h>

#define ASM_VERSION "v2.1"
#define SIGNATURE "SA"
#define ASM_NAME "ASM"
#define INIT_LABEL_NUMBER 32


static const char defaultOutput[] = "output.txt";
static const char defaultInput[] = "poem.txt";
static const char labelSign[] = ":";


static const char *const errList[] = {"",
                                      "Invalid arguments in function.",
                                      "Certain problems related to opening file.",
                                      "System couldn't allocate memory.",
                                      "Error, unknown flag.",
                                      "Strtoul couldn't interpret a token as a number.",
                                      "A label (without value) was the last token.",
                                      "One label was used twice."
};

/**
* The list of possible errors
*/

enum error_codes
{
    ARGUMENTS_ERR = 1,      //!< Means that function argument or some of them are equal NULL.
    OPENFILE_ERR,           //!< Means that some problem was detected in process of opening file.
    MEMORY_ERR,             //!< Means that system couldn't allocate memory.
    UNKNOWN_FLAG_ERR,       //!< Means that unknown flag was detected in the command line, possible options: -D, -A
    LABEL_VAL_ERR,          //!< Means that a number wasn't found after the labels symbol ( : )
    LABEL_EOF,              //!< Means that a label was the last token (without number of this label).
    LABEL_REPEAT,           //!< Means that
    OUT_OF_ERR_RANGE_ERR    //!< Means that num of the error is too large

};


typedef struct
{
    char *buffer;
    char **tokens;

    unsigned int *labels;
    unsigned int tokensNumber;
    unsigned int labelsNumber;

    double *binaryCode;
    size_t size;

} code_t;

int throw_error(unsigned int err_num, const char *usr_msg, const char *err_msg, const char *_func, int _line,
                const char *_file);

size_t countTokens(const char *buffer);

int getBuf(code_t *data, const char *inputFile);

int dumpCode_t(code_t *data);

int displayTokens(code_t *data);

int processAsmCode(code_t *data, const char *label);

int assemble(code_t *data);

int writeCode(code_t *data, const char *_outFilename);

void destructCode_t(code_t *code);

#endif //ASSEMBLER_ASSEMBLER_H
