//
// Created by superstraz on 7/19/18.
//

#ifndef ASSEMBLER_ASSEMBLER_H
#define ASSEMBLER_ASSEMBLER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define ASM_VERSION "v2.1"
#define SIGNATURE "SA"
#define ASM_NAME "ASM"
#define INIT_LABEL_NUMBER 32


typedef struct
{
    char *buffer;
    char **tokens;

    unsigned int *labels;
    unsigned int tokensNum;
    unsigned int labelsNum;

    size_t size;

} code_t;


size_t countTokens(const char *buffer);

int getBuf(code_t *data, const char *inputFile);

int displayText(code_t *data);

int makeTokens(code_t *data);

void assemble(char **const _arrayPtrCmd, double *const machCode, size_t sizeOfMachCode, char *labelArr);

void *writeCode(const double *machCode, const size_t sizeOfMachCode, const char *_outFilename);

void destructCode_t(code_t *code);

#endif //ASSEMBLER_ASSEMBLER_H
